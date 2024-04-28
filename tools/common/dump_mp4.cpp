/*
 * Copyright (c) 2003 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "dump_mp4.hpp"

#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/mathematics.h>
#include <libavutil/opt.h>
#include <libavutil/timestamp.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#define STREAM_DURATION 10.0
#define STREAM_FRAME_RATE 25 /* 25 images/s */

// a wrapper around a single output AVStream
typedef struct OutputStream {
  AVStream *st;
  AVCodecContext *enc;

  /* pts of the next frame that will be generated */
  int64_t next_pts;
  int samples_count;

  AVFrame *frame;
  AVFrame *tmp_frame;

  AVPacket *tmp_pkt;

  float t, tincr, tincr2;

  struct SwsContext *sws_ctx;
  struct SwrContext *swr_ctx;
} OutputStream;

static int write_frame(AVFormatContext *fmt_ctx, AVCodecContext *c,
                       AVStream *st, AVFrame *frame, AVPacket *pkt) {
  int ret;

  // send the frame to the encoder
  ret = avcodec_send_frame(c, frame);
  if (ret < 0) {
    fprintf(stderr, "Error sending a frame to the encoder: %s\n",
            av_err2str(ret));
    exit(1);
  }

  while (ret >= 0) {
    ret = avcodec_receive_packet(c, pkt);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
      break;
    else if (ret < 0) {
      fprintf(stderr, "Error encoding a frame: %s\n", av_err2str(ret));
      exit(1);
    }

    /* rescale output packet timestamp values from codec to stream timebase */
    av_packet_rescale_ts(pkt, c->time_base, st->time_base);
    pkt->stream_index = st->index;

    /* Write the compressed frame to the media file. */
    ret = av_interleaved_write_frame(fmt_ctx, pkt);
    /* pkt is now blank (av_interleaved_write_frame() takes ownership of
     * its contents and resets pkt), so that no unreferencing is necessary.
     * This would be different if one used av_write_frame(). */
    if (ret < 0) {
      fprintf(stderr, "Error while writing output packet: %s\n",
              av_err2str(ret));
      exit(1);
    }
  }

  return ret == AVERROR_EOF ? 1 : 0;
}

/* Add an output stream. */
static void add_stream(OutputStream *ost, AVFormatContext *oc,
                       const AVCodec **codec, enum AVCodecID codec_id) {
  AVCodecContext *c;
  int i;

  /* find the encoder */
  *codec = avcodec_find_encoder(codec_id);
  if (!(*codec)) {
    fprintf(stderr, "Could not find encoder for '%s'\n",
            avcodec_get_name(codec_id));
    exit(1);
  }

  ost->tmp_pkt = av_packet_alloc();
  if (!ost->tmp_pkt) {
    fprintf(stderr, "Could not allocate AVPacket\n");
    exit(1);
  }

  ost->st = avformat_new_stream(oc, NULL);
  if (!ost->st) {
    fprintf(stderr, "Could not allocate stream\n");
    exit(1);
  }
  ost->st->id = oc->nb_streams - 1;
  c = avcodec_alloc_context3(*codec);
  if (!c) {
    fprintf(stderr, "Could not alloc an encoding context\n");
    exit(1);
  }
  ost->enc = c;

  const AVChannelLayout temp = AV_CHANNEL_LAYOUT_STEREO;
  switch ((*codec)->type) {
    case AVMEDIA_TYPE_AUDIO:
      c->sample_fmt =
          (*codec)->sample_fmts ? (*codec)->sample_fmts[0] : AV_SAMPLE_FMT_FLTP;
      c->bit_rate = 64000;
      c->sample_rate = 44100;
      if ((*codec)->supported_samplerates) {
        c->sample_rate = (*codec)->supported_samplerates[0];
        for (i = 0; (*codec)->supported_samplerates[i]; i++) {
          if ((*codec)->supported_samplerates[i] == 44100)
            c->sample_rate = 44100;
        }
      }

#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(58, 0, 0)
      av_channel_layout_copy(&c->ch_layout, &temp);
#else
      c->channels = av_get_channel_layout_nb_channels(c->channel_layout);
      c->channel_layout = temp;
      if ((*codec)->channel_layouts) {
        c->channel_layout = (*codec)->channel_layouts[0];
        for (i = 0; (*codec)->channel_layouts[i]; i++) {
          if ((*codec)->channel_layouts[i] == temp)
            c->channel_layout = AV_CH_LAYOUT_STEREO;
        }
      }
      c->channels = av_get_channel_layout_nb_channels(c->channel_layout);
#endif
      ost->st->time_base = (AVRational){1, c->sample_rate};
      break;

    case AVMEDIA_TYPE_VIDEO:
      c->codec_id = codec_id;

      c->bit_rate = 8388608;
      /* Resolution must be a multiple of two. */
      c->width = 1440;
      c->height = 1080;
      /* timebase: This is the fundamental unit of time (in seconds) in terms
       * of which frame timestamps are represented. For fixed-fps content,
       * timebase should be 1/framerate and timestamp increments should be
       * identical to 1. */
      ost->st->time_base = (AVRational){1, STREAM_FRAME_RATE};
      c->time_base = ost->st->time_base;

      c->gop_size = 12; /* emit one intra frame every twelve frames at most */
      c->pix_fmt = AV_PIX_FMT_YUV420P;
      if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
        /* just for testing, we also add B-frames */
        c->max_b_frames = 2;
      }
      if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
        /* Needed to avoid using macroblocks in which some coeffs overflow.
         * This does not happen with normal video, it just happens here as
         * the motion of the chroma plane does not match the luma plane. */
        c->mb_decision = 2;
      }
      break;
    default:
      break;
  }

  /* Some formats want stream headers to be separate. */
  if (oc->oformat->flags & AVFMT_GLOBALHEADER)
    c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

/**************************************************************/
/* audio output */

#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(58, 0, 0)
static AVFrame *alloc_audio_frame(enum AVSampleFormat sample_fmt,
                                  const AVChannelLayout *channel_layout,
                                  int sample_rate, int nb_samples) {
#else
static AVFrame *alloc_audio_frame(enum AVSampleFormat sample_fmt,
                                  uint64_t channel_layout, int sample_rate,
                                  int nb_samples) {
#endif
  AVFrame *frame = av_frame_alloc();
  if (!frame) {
    fprintf(stderr, "Error allocating an audio frame\n");
    exit(1);
  }

  frame->format = sample_fmt;
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(58, 0, 0)
  av_channel_layout_copy(&frame->ch_layout, channel_layout);
#else
  frame->channel_layout = channel_layout;
#endif

  frame->sample_rate = sample_rate;
  frame->nb_samples = nb_samples;

  if (nb_samples) {
    if (av_frame_get_buffer(frame, 0) < 0) {
      fprintf(stderr, "Error allocating an audio buffer\n");
      exit(1);
    }
  }

  return frame;
}

static void open_audio(AVFormatContext *oc, const AVCodec *codec,
                       OutputStream *ost, AVDictionary *opt_arg) {
  AVCodecContext *c;
  int nb_samples;
  int ret;
  AVDictionary *opt = NULL;

  c = ost->enc;

  /* open it */
  av_dict_copy(&opt, opt_arg, 0);
  ret = avcodec_open2(c, codec, &opt);
  av_dict_free(&opt);
  if (ret < 0) {
    fprintf(stderr, "Could not open audio codec: %s\n", av_err2str(ret));
    exit(1);
  }

  /* init signal generator */
  ost->t = 0;
  ost->tincr = 2 * M_PI * 110.0 / c->sample_rate;
  /* increment frequency by 110 Hz per second */
  ost->tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;

  if (c->codec->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE)
    nb_samples = 10000;
  else
    nb_samples = c->frame_size;

#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(58, 0, 0)
  ost->frame = alloc_audio_frame(c->sample_fmt, &c->ch_layout, c->sample_rate,
                                 nb_samples);
  ost->tmp_frame = alloc_audio_frame(AV_SAMPLE_FMT_S16, &c->ch_layout,
                                     c->sample_rate, nb_samples);
#else
  ost->frame = alloc_audio_frame(c->sample_fmt, c->channel_layout, c->sample_rate,
                                 nb_samples);
  ost->tmp_frame = alloc_audio_frame(AV_SAMPLE_FMT_S16, c->channel_layout,
                                     c->sample_rate, nb_samples);
#endif

  /* copy the stream parameters to the muxer */
  ret = avcodec_parameters_from_context(ost->st->codecpar, c);
  if (ret < 0) {
    fprintf(stderr, "Could not copy the stream parameters\n");
    exit(1);
  }

  /* create resampler context */
  ost->swr_ctx = swr_alloc();
  if (!ost->swr_ctx) {
    fprintf(stderr, "Could not allocate resampler context\n");
    exit(1);
  }

  /* set options */
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(58, 0, 0)
  av_opt_set_chlayout(ost->swr_ctx, "in_chlayout", &c->ch_layout, 0);
  av_opt_set_chlayout(ost->swr_ctx, "out_chlayout", &c->ch_layout, 0);
#else
  av_opt_set_int(ost->swr_ctx, "in_channel_count", c->channels, 0);
  av_opt_set_int(ost->swr_ctx, "out_channel_count", c->channels, 0);
#endif
  av_opt_set_int(ost->swr_ctx, "in_sample_rate", c->sample_rate, 0);
  av_opt_set_sample_fmt(ost->swr_ctx, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
  av_opt_set_int(ost->swr_ctx, "out_sample_rate", c->sample_rate, 0);
  av_opt_set_sample_fmt(ost->swr_ctx, "out_sample_fmt", c->sample_fmt, 0);

  /* initialize the resampling context */
  if ((ret = swr_init(ost->swr_ctx)) < 0) {
    fprintf(stderr, "Failed to initialize the resampling context\n");
    exit(1);
  }
}

/* Prepare a 16 bit dummy audio frame of 'frame_size' samples and
 * 'nb_channels' channels. */
static AVFrame *get_audio_frame(OutputStream *ost) {
  AVFrame *frame = ost->tmp_frame;
  int j, i, v;
  int16_t *q = (int16_t *)frame->data[0];

  /* check if we want to generate more frames */
  if (av_compare_ts(ost->next_pts, ost->enc->time_base, STREAM_DURATION,
                    (AVRational){1, 1}) > 0)
    return NULL;

  for (j = 0; j < frame->nb_samples; j++) {
    v = (int)(sin(ost->t) * 10000);
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(58, 0, 0)
    for (i = 0; i < ost->enc->ch_layout.nb_channels; i++) *q++ = v;
#else
    for (i = 0; i < ost->enc->channels; i++) *q++ = v;
#endif
    ost->t += ost->tincr;
    ost->tincr += ost->tincr2;
  }

  frame->pts = ost->next_pts;
  ost->next_pts += frame->nb_samples;

  return frame;
}

/*
 * encode one audio frame and send it to the muxer
 * return 1 when encoding is finished, 0 otherwise
 */
static int write_audio_frame(AVFormatContext *oc, OutputStream *ost) {
  AVCodecContext *c;
  AVFrame *frame;
  int ret;
  int dst_nb_samples;

  c = ost->enc;

  frame = get_audio_frame(ost);

  if (frame) {
    /* convert samples from native format to destination codec format, using the
     * resampler */
    /* compute destination number of samples */
    dst_nb_samples = av_rescale_rnd(
        swr_get_delay(ost->swr_ctx, c->sample_rate) + frame->nb_samples,
        c->sample_rate, c->sample_rate, AV_ROUND_UP);
    av_assert0(dst_nb_samples == frame->nb_samples);

    /* when we pass a frame to the encoder, it may keep a reference to it
     * internally;
     * make sure we do not overwrite it here
     */
    ret = av_frame_make_writable(ost->frame);
    if (ret < 0) exit(1);

    /* convert to destination format */
    ret = swr_convert(ost->swr_ctx, ost->frame->data, dst_nb_samples,
                      (const uint8_t **)frame->data, frame->nb_samples);
    if (ret < 0) {
      fprintf(stderr, "Error while converting\n");
      exit(1);
    }
    frame = ost->frame;

    frame->pts = av_rescale_q(ost->samples_count,
                              (AVRational){1, c->sample_rate}, c->time_base);
    ost->samples_count += dst_nb_samples;
  }

  return write_frame(oc, c, ost->st, frame, ost->tmp_pkt);
}

/**************************************************************/
/* video output */

static AVFrame *alloc_frame(enum AVPixelFormat pix_fmt, int width, int height) {
  AVFrame *frame;
  int ret;

  frame = av_frame_alloc();
  if (!frame) return NULL;

  frame->format = pix_fmt;
  frame->width = width;
  frame->height = height;

  /* allocate the buffers for the frame data */
  ret = av_frame_get_buffer(frame, 0);
  if (ret < 0) {
    fprintf(stderr, "Could not allocate frame data.\n");
    exit(1);
  }

  return frame;
}

static void open_video(AVFormatContext *oc, const AVCodec *codec,
                       OutputStream *ost, AVDictionary *opt_arg) {
  int ret;
  AVCodecContext *c = ost->enc;
  AVDictionary *opt = NULL;

  av_dict_copy(&opt, opt_arg, 0);

  /* open the codec */
  ret = avcodec_open2(c, codec, &opt);
  av_dict_free(&opt);
  if (ret < 0) {
    fprintf(stderr, "Could not open video codec: %s\n", av_err2str(ret));
    exit(1);
  }

  /* allocate and init a re-usable frame */
  ost->frame = alloc_frame(c->pix_fmt, c->width, c->height);
  if (!ost->frame) {
    fprintf(stderr, "Could not allocate video frame\n");
    exit(1);
  }

  /* If the output format is not YUV420P, then a temporary YUV420P
   * picture is needed too. It is then converted to the required
   * output format. */
  ost->tmp_frame = NULL;
  if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
    ost->tmp_frame = alloc_frame(AV_PIX_FMT_YUV420P, c->width, c->height);
    if (!ost->tmp_frame) {
      fprintf(stderr, "Could not allocate temporary video frame\n");
      exit(1);
    }
  }

  /* copy the stream parameters to the muxer */
  ret = avcodec_parameters_from_context(ost->st->codecpar, c);
  if (ret < 0) {
    fprintf(stderr, "Could not copy the stream parameters\n");
    exit(1);
  }
}

/* Prepare a dummy image. */
static void fill_yuv_image(AVFrame *pict, int frame_index, int width,
                           int height) {
  int x, y, i;

  i = frame_index;

  /* Y */
  for (y = 0; y < height; y++)
    for (x = 0; x < width; x++)
      pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;

  /* Cb and Cr */
  for (y = 0; y < height / 2; y++) {
    for (x = 0; x < width / 2; x++) {
      pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
      pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
    }
  }
}

static AVFrame *get_video_frame(OutputStream *ost) {
  AVCodecContext *c = ost->enc;

  /* check if we want to generate more frames */
  if (av_compare_ts(ost->next_pts, c->time_base, STREAM_DURATION,
                    (AVRational){1, 1}) > 0)
    return NULL;

  /* when we pass a frame to the encoder, it may keep a reference to it
   * internally; make sure we do not overwrite it here */
  if (av_frame_make_writable(ost->frame) < 0) exit(1);

  if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
    /* as we only generate a YUV420P picture, we must convert it
     * to the codec pixel format if needed */
    if (!ost->sws_ctx) {
      ost->sws_ctx =
          sws_getContext(c->width, c->height, AV_PIX_FMT_YUV420P, c->width,
                         c->height, c->pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);
      if (!ost->sws_ctx) {
        fprintf(stderr, "Could not initialize the conversion context\n");
        exit(1);
      }
    }
    fill_yuv_image(ost->tmp_frame, ost->next_pts, c->width, c->height);
    sws_scale(ost->sws_ctx, (const uint8_t *const *)ost->tmp_frame->data,
              ost->tmp_frame->linesize, 0, c->height, ost->frame->data,
              ost->frame->linesize);
  } else {
    fill_yuv_image(ost->frame, ost->next_pts, c->width, c->height);
  }

  ost->frame->pts = ost->next_pts++;

  return ost->frame;
}

/*
 * encode one video frame and send it to the muxer
 * return 1 when encoding is finished, 0 otherwise
 */
static int write_video_frame(AVFormatContext *oc, OutputStream *ost) {
  return write_frame(oc, ost->enc, ost->st, get_video_frame(ost), ost->tmp_pkt);
}

static void close_stream(AVFormatContext *oc, OutputStream *ost) {
  avcodec_free_context(&ost->enc);
  av_frame_free(&ost->frame);
  av_frame_free(&ost->tmp_frame);
  av_packet_free(&ost->tmp_pkt);
  sws_freeContext(ost->sws_ctx);
  swr_free(&ost->swr_ctx);
}

int Dump_mp4(Pixel *data, int width, int height, const char *filename) {
  OutputStream video_st = {0}, audio_st = {0};
  const AVOutputFormat *fmt;
  AVFormatContext *oc;
  const AVCodec *audio_codec, *video_codec;
  int ret;
  int have_video = 0, have_audio = 0;
  int encode_video = 0, encode_audio = 0;
  AVDictionary *opt = NULL;
  int i;

  /* allocate the output media context */
  avformat_alloc_output_context2(&oc, NULL, NULL, filename);
  if (!oc) return 1;

  fmt = oc->oformat;

  /* Add the audio and video streams using the default format codecs
   * and initialize the codecs. */
  if (fmt->video_codec != AV_CODEC_ID_NONE) {
    add_stream(&video_st, oc, &video_codec, fmt->video_codec);
    have_video = 1;
    encode_video = 1;
  }
  if (fmt->audio_codec != AV_CODEC_ID_NONE) {
    add_stream(&audio_st, oc, &audio_codec, fmt->audio_codec);
    have_audio = 1;
    encode_audio = 1;
  }

  /* Now that all the parameters are set, we can open the audio and
   * video codecs and allocate the necessary encode buffers. */
  if (have_video) open_video(oc, video_codec, &video_st, opt);

  if (have_audio) open_audio(oc, audio_codec, &audio_st, opt);

  av_dump_format(oc, 0, filename, 1);

  /* open the output file, if needed */
  if (!(fmt->flags & AVFMT_NOFILE)) {
    ret = avio_open(&oc->pb, filename, AVIO_FLAG_WRITE);
    if (ret < 0) {
      fprintf(stderr, "Could not open '%s': %s\n", filename, av_err2str(ret));
      return 1;
    }
  }

  /* Write the stream header, if any. */
  ret = avformat_write_header(oc, &opt);
  if (ret < 0) {
    fprintf(stderr, "Error occurred when opening output file: %s\n",
            av_err2str(ret));
    return 1;
  }

  while (encode_video || encode_audio) {
    /* select the stream to encode */
    if (encode_video &&
        (!encode_audio ||
         av_compare_ts(video_st.next_pts, video_st.enc->time_base,
                       audio_st.next_pts, audio_st.enc->time_base) <= 0)) {
      encode_video = !write_video_frame(oc, &video_st);
    } else {
      encode_audio = !write_audio_frame(oc, &audio_st);
    }
  }

  av_write_trailer(oc);

  /* Close each codec. */
  if (have_video) close_stream(oc, &video_st);
  if (have_audio) close_stream(oc, &audio_st);

  if (!(fmt->flags & AVFMT_NOFILE)) /* Close the output file. */
    avio_closep(&oc->pb);

  /* free the stream */
  avformat_free_context(oc);
  return 0;
}

#ifndef DUMP_MP4_HPP
#define DUMP_MP4_HPP

typedef unsigned int Pixel;

int Dump_mp4(Pixel* data,int width,int height,const char* filename);
int Read_mp4(Pixel*& data,int& width,int& height,const char* filename);

#endif // DUMP_MP4_HPP

#ifndef DUMP_MP4_HPP
#define DUMP_MP4_HPP

typedef unsigned int Pixel;

int Dump_mp4(const char *filename, Pixel **data, int width, int height, int frames, int framerate, int framerateDenominator = 1);

#endif  // DUMP_MP4_HPP

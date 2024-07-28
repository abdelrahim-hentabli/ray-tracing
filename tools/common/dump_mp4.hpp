#ifndef DUMP_MP4_HPP
#define DUMP_MP4_HPP

typedef unsigned int Pixel;

#define R(pixel) (pixel >> 24)
#define G(pixel) ((pixel >> 16) & 0xFF)
#define B(pixel) ((pixel >> 8) & 0xFF)


int Dump_mp4(const char *filename, Pixel **data, int width, int height, int frames, int framerate, int framerateDenominator = 1);

#endif  // DUMP_MP4_HPP

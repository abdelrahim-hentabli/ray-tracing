#pragma once

typedef unsigned int Pixel;

#define R(pixel) (pixel >> 24)
#define G(pixel) ((pixel >> 16) & 0xFF)
#define B(pixel) ((pixel >> 8) & 0xFF)

int Dump_mp4(const char* filename, Pixel** data, int width, int height,
             int frames, int framerate, int framerateDenominator = 1);

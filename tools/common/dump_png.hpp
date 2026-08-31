#pragma once

typedef unsigned int Pixel;

void Dump_png(Pixel* data, int width, int height, const char* filename);
void Read_png(Pixel*& data, int& width, int& height, const char* filename);

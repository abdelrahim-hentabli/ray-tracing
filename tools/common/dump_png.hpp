#ifndef DUMP_PNG_HPP
#define DUMP_PNG_HPP

typedef unsigned int Pixel;

void Dump_png(Pixel *data, int width, int height, const char *filename);
void Read_png(Pixel *&data, int &width, int &height, const char *filename);

#endif  // DUMP_PNG_HPP
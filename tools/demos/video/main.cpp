#include <iostream>
#include <cstdio>
#include <unistd.h>
// ray_tracer
#include "render_world.hpp"
#include "objects/object.hpp"
// tool_common
#include "parse.hpp"
#include "dump_mp4.hpp"

void Usage(const char* exec) {
    std::cerr<<"Usage: "<<exec<<" -i <test-file> [ -s <solution-file> ] [ -o <stats-file> ] [ -x <debug-x-coord> -y <debug-y-coord> ]"<<std::endl;
    exit(1);
}

int main(int argc, char** argv) {
    Pixel* data = nullptr;
    Dump_mp4(data, 352, 288, "output.mp4");
}

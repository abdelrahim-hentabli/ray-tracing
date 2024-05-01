#include <unistd.h>

#include <cstdio>
#include <iostream>
// ray_tracer
#include "objects/object.hpp"
#include "render_world.hpp"
// tool_common
#include "dump_mp4.hpp"
#include "parse.hpp"

void Usage(const char *exec) {
  std::cerr << "Usage: " << exec
            << " -i <test-file>"
            << std::endl;
  exit(1);
}

int main(int argc, char **argv) {
  const char *input_file = 0;
  Pixel *data = nullptr;

  // Parse commandline options
  while (1) {
    int opt = getopt(argc, argv, "s:i:m:o:x:y:h");
    if (opt == -1) break;
    switch (opt) {
      case 'i':
        input_file = optarg;
        break;
      default:
        break;
    }
  }
  if (!input_file) Usage(argv[0]);

  int width = 0;
  int height = 0;
  Render_World world;

  // Parse test scene file
  Parse(world, width, height, input_file);

  // Render the image
  world.Render();
  Dump_mp4(data, width, height, "output.mp4");
}

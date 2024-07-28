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
  std::cerr << "Usage: " << exec << " -i <test-file>" << std::endl;
  exit(1);
}

int main(int argc, char **argv) {
  const char *input_file = 0;
  Pixel **data = nullptr;

  // Parse commandline options
  while (1) {
    int opt = getopt(argc, argv, "i:");
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

  constexpr int FPS       = 24;
  constexpr int SECONDS   = 3;
  constexpr double DELTA  = double (FPS) / double (SECONDS);

  data = new Pixel*[(FPS * SECONDS) +1];

  for (int i = 0; i <= FPS * SECONDS; i++) {
    world.Render();
    data[i] = world.camera.colors;
    world.camera.Clear_Camera();
    world.Update(DELTA);
  }

  Dump_mp4("output.mp4", data, width, height, SECONDS * FPS + 1, FPS, 1);
}

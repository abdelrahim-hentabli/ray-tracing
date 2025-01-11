#ifndef TEST_FILE_HPP
#define TEST_FILE_HPP

#include "directories.hpp"
#include "dump_png.hpp"
#include "gtest/gtest.h"
#include "lights/point_light.hpp"
#include "objects/mesh.hpp"
#include "parse.hpp"

void test_file(std::string case_name) {
  int width = 0;
  int height = 0;
  Render_World world;

  // Parse test scene file
  Parse(world, width, height, (getCasesDir() + case_name + ".txt").c_str());

  // Render the image
  world.Render();

  int width2 = 0, height2 = 0;
  Pixel *data_sol = 0;

  // Read solution from disk
  Read_png(data_sol, width2, height2,
           (getTestSolutionsDir() + case_name + ".png").c_str());
  assert(width == width2);
  assert(height == height2);

  // For each pixel, check to see if it matches solution
  double error = 0, total = 0;
  for (int i = 0; i < height * width; i++) {
    vec3 a = From_Pixel(world.camera.colors[i]);
    vec3 b = From_Pixel(data_sol[i]);
    for (int c = 0; c < 3; c++) {
      double e = fabs(a[c] - b[c]);
      error += e;
      total++;
      b[c] = e;
    }
    data_sol[i] = Pixel_Color(b);
  }

  double diff = error / total * 100;
  EXPECT_NEAR(0.00, diff, 1e-3);

  delete[] data_sol;
}

TEST(simple, test_0) { test_file("00"); }
TEST(simple, test_1) { test_file("01"); }
TEST(simple, test_2) { test_file("02"); }
TEST(simple, test_3) { test_file("03"); }
TEST(simple, test_4) { test_file("04"); }
TEST(simple, test_5) { test_file("05"); }
TEST(simple, test_6) { test_file("06"); }
TEST(simple, test_7) { test_file("07"); }
TEST(simple, test_8) { test_file("08"); }
TEST(simple, test_9) { test_file("09"); }
TEST(simple, test_10) { test_file("10"); }
TEST(simple, test_11) { test_file("11"); }
TEST(simple, test_12) { test_file("12"); }
TEST(simple, test_13) { test_file("13"); }
TEST(simple, test_14) { test_file("14"); }
TEST(simple, test_15) { test_file("15"); }
TEST(simple, test_16) { test_file("16"); }
TEST(simple, test_17) { test_file("17"); }
TEST(simple, test_18) { test_file("18"); }
TEST(simple, test_19) { test_file("19"); }
TEST(simple, test_20) { test_file("20"); }
TEST(simple, test_21) { test_file("21"); }
TEST(simple, test_22) { test_file("22"); }
TEST(simple, test_23) { test_file("23"); }
TEST(simple, test_24) { test_file("24"); }
TEST(simple, test_25) { test_file("25"); }
TEST(simple, test_26) { test_file("26"); }
TEST(simple, test_27) { test_file("27"); }
TEST(simple, test_28) { test_file("28"); }
TEST(simple, test_29) { test_file("29"); }

Render_World SetupBenchmarkWorld(int width, int height, vec3 cameraP,
                                 vec3 cameraL, vec3 cameraU) {
  Render_World world;

  // Setup world
  world.enable_shadows = false;
  world.recursion_depth_limit = 1;
  shader_data default_bg;
  default_bg.type = flat_shader;
  world.sd = default_bg;

  // Setup objects
  Mesh *o = new Mesh;
  o->Read_Obj("sphere.obj");
  shader_data temp;
  temp.type = phong_shader;
  temp.color_ambient = {1, 1, 1};
  temp.color_diffuse = {1, 1, 1};
  temp.color_specular = {1, 1, 1};
  temp.specular_power = 50;
  o->sd = temp;
  world.objects.push_back(o);

  // Setup lights
  world.lights.push_back(new Point_Light(vec3(.8, .8, 4), vec3(1, 1, 1), 100));
  world.ambient_color = {1, 1, 1};
  world.ambient_intensity = 0;

  // Setup Camera
  world.camera.Position_And_Aim_Camera(cameraP, cameraL, cameraU);
  world.camera.Focus_Camera(1, (double)width / height, 70 * (pi / 180));

  world.camera.Set_Resolution(ivec2(width, height));

  return world;
}

void test_benchmark() {
  int width = 640;
  int height = 480;
  Render_World world = SetupBenchmarkWorld(width, height, vec3(0, 0, 2),
                                           vec3(0, 0, 0), vec3(0, 1, 0));

  // Render the image
  world.Render();

  // Compare agains stored image for test
  int width2 = 0, height2 = 0;
  Pixel *data_sol = 0;

  // Read solution from disk
  Read_png(data_sol, width2, height2,
           (getTestSolutionsDir() + "bench.png").c_str());
  assert(width == width2);
  assert(height == height2);

  // For each pixel, check to see if it matches solution
  double error = 0, total = 0;
  for (int i = 0; i < height * width; i++) {
    vec3 a = From_Pixel(world.camera.colors[i]);
    vec3 b = From_Pixel(data_sol[i]);
    for (int c = 0; c < 3; c++) {
      double e = fabs(a[c] - b[c]);
      error += e;
      total++;
      b[c] = e;
    }
    data_sol[i] = Pixel_Color(b);
  }

  double diff = error / total * 100;
  EXPECT_NEAR(0.00, diff, 1e-3);

  delete[] data_sol;
}

TEST(simple, bench) { test_benchmark(); }
#endif  // TEST_FILE_HPP

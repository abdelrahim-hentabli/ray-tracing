#ifndef __SHADER_H__
#define __SHADER_H__

#include "objects/object.hpp"
#include "ray.hpp"
#include "render_world.hpp"
#include "vec.hpp"

extern bool debug_pixel;

enum shader_type {
  flat_shader = 1U,
  phong_shader = 2U,
  reflective_flat = 11U,
  reflective_phong = 12U,
  refractive_flat = 21U,
  refractive_phong = 22U,
};

vec3 Shade_Surface(shader_type type, const Ray &ray,
                   const vec3 &intersection_point, const vec3 &normal,
                   int recursion_depth, const Render_World &world,
                   const vec3 &color_ambient, const vec3 &color_diffuse,
                   const vec3 &color_specular, double specular_power,
                   double incidence_of_refraction, double color_intensity);

class Shader {
 public:
  Render_World &world;

  Shader(Render_World &world_input) : world(world_input) {}

  virtual ~Shader() {}

  virtual vec3 Shade_Surface(const Ray &ray, const vec3 &intersection_point,
                             const vec3 &normal, int recursion_depth) const = 0;
};

#endif

#ifndef __TYPES_H__
#define __TYPES_H__

#include "vec.hpp"

enum shader_type {
  flat_shader = 1U,
  phong_shader = 2U,
  reflective_flat = 11U,
  reflective_phong = 12U,
  refractive_flat = 21U,
  refractive_phong = 22U,
};

struct shader_data {
  shader_type type;
  vec3 color_ambient;
  vec3 color_diffuse;
  vec3 color_specular;
  double specular_power;
  double incidence_of_refraction;
  double color_intensity;
};

#endif  //__TYPES_H__

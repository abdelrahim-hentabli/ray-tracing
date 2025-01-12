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
  vec3 color_diffuse;              // phong based
  vec3 color_specular;             // phong based
  double specular_power;           // phong based
  double incidence_of_refraction;  // refraction shader
  double color_intensity;          // reflect/refract
};

enum light_type {
  point_light = 1U,
  direction_light = 2U,
  spot_light = 3U,
};

struct light_data {
  light_type type;
  vec3 position;
  vec3 color;
  double brightness;
  double min_cos_angle;     // Spot Light
  double falloff_exponent;  // Spot Light
  vec3 direction;           // Spot Light
};

#endif  //__TYPES_H__

#ifndef __REFRACTIVE_SHADER_H__
#define __REFRACTIVE_SHADER_H__

#include <algorithm>

#include "shaders/shader.hpp"

class Refractive_Shader : public Shader {
 public:
  Shader *shader;
  double incidence_of_refraction;
  double color_intensity;

  Refractive_Shader(Render_World &world_input, Shader *shader_input,
                    double ior, double ci)
      : Shader(world_input),
        shader(shader_input),
        incidence_of_refraction(ior),
        color_intensity(ci) {}

  virtual vec3 Shade_Surface(const Ray &ray, const vec3 &intersection_point,
                             const vec3 &normal,
                             int recursion_depth) const override;

private:
  void fresnel(const vec3 &direction, const vec3 &normal, double &kr) const;
  vec3 reflect(const vec3 &direction, const vec3 &normal) const;
  vec3 refract(const vec3 &direction, const vec3 &normal) const;
};
#endif

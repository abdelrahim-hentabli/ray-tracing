#ifndef __SHADER_H__
#define __SHADER_H__

#include "ray.hpp"
#include "types.hpp"
#include "vec.hpp"

extern bool debug_pixel;

class Render_World;

vec3 Shade_Surface(const Ray &ray, const vec3 &intersection_point,
                   const vec3 &normal, int recursion_depth,
                   const Render_World &world, const shader_data &sd);

#endif

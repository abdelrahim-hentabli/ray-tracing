#ifndef __POINT_LIGHT_H__
#define __POINT_LIGHT_H__

#include <math.h>

#include <iostream>
#include <limits>
#include <vector>

#include "light.hpp"
#include "vec.hpp"

class Point_Light : public Light {
 public:
  Point_Light(const vec3 &position, const vec3 &color, double brightness)
      : Light(position, color, brightness) {}

  vec3 Emitted_Light(const vec3 &vector_to_light) const {
    return color * brightness / (4 * pi * vector_to_light.magnitude_squared());
  }
};
#endif

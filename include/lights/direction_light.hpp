#ifndef __DIRECTION_LIGHT_H__
#define __DIRECTION_LIGHT_H__

#include <math.h>

#include <iostream>
#include <limits>
#include <vector>

#include "light.hpp"
#include "vec.hpp"

class Direction_Light : public Light {
 public:
  Direction_Light(const vec3 &direction, const vec3 &color, double brightness)
      : Light(direction.normalized() * 1e10, color, brightness) {}

  vec3 Emitted_Light(const vec3 &vector_to_light) const {
    return color * brightness;
  }
};
#endif

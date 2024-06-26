#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <math.h>

#include <iostream>
#include <limits>
#include <vector>

#include "vec.hpp"

class Ray;

class Light {
 public:
  vec3 position;
  vec3 color;  // RGB color components
  double brightness;

  Light() : position(), color(1, 1, 1), brightness(1) {}

  Light(const vec3 &position, const vec3 &color, double brightness)
      : position(position), color(color), brightness(brightness) {}

  virtual ~Light() {}

  virtual vec3 Emitted_Light(const vec3 &vector_to_light) const = 0;
};
#endif

#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <math.h>

#include <iostream>
#include <limits>
#include <vector>

#include "types.hpp"
#include "vec.hpp"
#include "misc.hpp"

class Ray;

vec3 Emitted_Light(const vec3 &vector_to_light, const light_data& ld);

#endif

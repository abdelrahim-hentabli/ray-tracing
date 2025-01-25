#pragma once

#include <math.h>

#include <iostream>
#include <limits>
#include <vector>

#include "misc.hpp"
#include "types.hpp"
#include "vec.hpp"

class Ray;

vec3 Emitted_Light(const vec3& vector_to_light, const light_data& ld);

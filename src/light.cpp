#include "lights/light.hpp"

vec3 Emitted_Light(const vec3& vector_to_light, const light_data& ld) {
  switch (ld.type) {
    case point_light:
      return ld.color * ld.brightness /
             (4 * pi * vector_to_light.magnitude_squared());
      break;
    case direction_light:
      return ld.color * ld.brightness;
      break;
    case spot_light:
      TODO;
      break;
    default:
      break;
  }
  return vec3();
}

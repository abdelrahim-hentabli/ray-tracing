#include "objects/plane.hpp"

#include <cfloat>
#include <limits>

#include "ray.hpp"

// Intersect with the half space defined by the plane.  The plane's normal
// points outside.  If the ray starts on the "inside" side of the plane, be sure
// to record a hit with t=0 as the first entry in hits.
Hit Plane::Intersection(const Ray &ray, int part) const {
  double product = dot(normal, ray.direction);
  if (product < -1e-6) {
    vec3 second_vec = x1 - ray.endpoint;
    double t = dot(second_vec, normal) / product;
    return {this, t, 0};
  }
  return {0, 0, 0};
}

vec3 Plane::Normal(const vec3 &point, int part) const { return normal; }

// There is not a good answer for the bounding box of an infinite object.
// The safe thing to do is to return a box that contains everything.
Box Plane::Bounding_Box(int part) const {
  Box b;
  b.hi.fill(std::numeric_limits<double>::max());
  b.lo = -b.hi;
  return b;
}

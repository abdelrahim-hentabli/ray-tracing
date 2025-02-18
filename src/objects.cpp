#include "objects/object.hpp"

constexpr double weight_tol = 1e-4;

New_Hit Intersection(const Ray &ray, const object_data &od) {
  switch (od.type) {
    case sphere: {  // Sphere Object
      vec3 L = od.position - ray.endpoint;
      double tc = dot(L, ray.direction);
      if (tc < 0.0) {
        return {0, 0};
      }
      float dist_to_center_squared = L.magnitude_squared() - tc * tc;
      if (abs(dist_to_center_squared) > od.radius * od.radius) {
        return {0, 0};
      }

      float t1c = sqrt(od.radius * od.radius - dist_to_center_squared);

      return {&od, tc - t1c};
    }
    case plane: {  // Plane Object
      double product = dot(od.normal, ray.direction);
      if (product < -1e-6) {
        vec3 second_vec = od.position - ray.endpoint;
        double t = dot(second_vec, od.normal) / product;
        return {&od, t};
      }
      return {0, 0};
    }

    case triangle: {  // Triangle Object
      vec3 normal =
          cross(od.position1 - od.position, od.position2 - od.position);

      double bespoke_weight_tolerance =
          normal.magnitude_squared() * -weight_tol;

      if (abs(dot(normal, ray.direction)) < -1e-4) {
        return {0, 0};
      }

      double d = -dot(od.normal, od.position);

      double t = -(dot(normal, ray.endpoint) + d) / dot(normal, ray.direction);
      if (t < small_t) {
        return {0, 0};
      }

      vec3 p = ray.endpoint + t * ray.direction;

      vec3 c = cross(od.position1 - od.position, p - od.position);
      if (dot(normal, c) < bespoke_weight_tolerance) {
        return {0, 0};
      }

      c = cross(od.position2 - od.position1, p - od.position1);
      if (dot(normal, c) < bespoke_weight_tolerance) {
        return {0, 0};
      }
      c = cross(od.position - od.position2, p - od.position2);
      if (dot(normal, c) < bespoke_weight_tolerance) {
        return {0, 0};
      }

      return {&od, t};
    }
    default:
      return {0, 0};
  }
}

vec3 Normal(const vec3 &point, const object_data &od) {
  switch (od.type) {
    case sphere:
      return (point - od.position).normalized();
    case plane:
      return od.normal;
    case triangle:
      return od.normal;
    default:
      return od.normal;
  }
}

Box Bounding_Box(const object_data &od) {
  Box box;
  switch (od.type) {
    case sphere:
      box.hi = od.position + vec3(od.radius, od.radius, od.radius);
      box.lo = od.position - vec3(od.radius, od.radius, od.radius);
      break;
    case plane:
      box.hi.fill(std::numeric_limits<double>::max());
      box.lo = -box.hi;
      break;
    case triangle:
      box.Include_Point(od.position);
      box.Include_Point(od.position1);
      box.Include_Point(od.position2);
      break;
    default:
      break;
  }
}

#ifndef __PLANE_H__
#define __PLANE_H__

#include "objects/object.hpp"

class Plane : public Object {
 public:
  vec3 x1;
  vec3 normal;
  // Describes updating the location and rotation of object
  vec3 velocity;
  vec3 acceleration;

  Plane(const vec3 &point, const vec3 &normal)
      : x1(point), normal(normal.normalized()) {}

  virtual Hit Intersection(const Ray &ray, int part) const override;
  virtual vec3 Normal(const vec3 &point, int part) const override;
  virtual Box Bounding_Box(int part) const override;
  void Update(float deltaT) override;

};

#endif

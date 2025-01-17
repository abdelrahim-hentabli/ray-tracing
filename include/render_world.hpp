#ifndef __RENDER_WORLD_H__
#define __RENDER_WORLD_H__

#include <vector>

#include "acceleration_structures/hierarchy.hpp"
#include "camera.hpp"
#include "types.hpp"

class Object;
class Ray;

class Render_World {
 public:
  Camera camera;

  shader_data sd;
  std::vector<Object *> objects;
  std::vector<light_data> lights;
  vec3 ambient_color;
  double ambient_intensity;

  bool enable_shadows;
  int recursion_depth_limit;

  Hierarchy hierarchy;
  bool hierarchy_initialized = false;

  Render_World();
  ~Render_World();
  Render_World(Render_World &&other) = default;
  Render_World &operator=(Render_World &&other) = default;

  Render_World(const Render_World &other) = delete;
  Render_World &operator=(const Render_World &other) = delete;

  void Render_Pixel(const ivec2 &pixel_index);
  void Render();
  void Initialize_Hierarchy();
  void Clear_Hierarchy();

  vec3 Cast_Ray(const Ray &ray, int recursion_depth);
  Hit Closest_Intersection(const Ray &ray) const;

  void Update(double deltaT);
};

#endif

#include "render_world.hpp"

#include <limits>

#include "acceleration_structures/hierarchy.hpp"
#include "lights/light.hpp"
#include "objects/object.hpp"
#include "ray.hpp"
#include "shaders/shader.hpp"

Render_World::Render_World()
    : ambient_intensity(0), enable_shadows(true), recursion_depth_limit(3) {}

Render_World::~Render_World() {
  for (size_t i = 0; i < objects.size(); i++) delete objects[i];
}

// Find and return the Hit structure for the closest intersection.  Be careful
// to ensure that hit.dist>=small_t.
Hit Render_World::Closest_Intersection(const Ray &ray) const {
  Hit temp;
  Hit output{nullptr, std::numeric_limits<double>::max(), 0};
  std::vector<int> candidates;
  hierarchy.Intersection_Candidates(ray, candidates);
  for (int candidate : candidates) {
    temp = hierarchy.entries[candidate].obj->Intersection(
        ray, hierarchy.entries[candidate].part);
    if (temp.object != nullptr) {
      if (temp.dist < output.dist) {
        output = temp;
      }
    }
  }
  return output;
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2 &pixel_index) {
  Ray ray = Ray(camera.position,
                camera.World_Position(pixel_index) - camera.position);
  vec3 color = Cast_Ray(ray, 1);
  camera.Set_Pixel(pixel_index, Pixel_Color(color));
}

void Render_World::Render() {
  if (!hierarchy_initialized) {
    Initialize_Hierarchy();
  }
  Ray ray;
#pragma omp parallel for num_threads(8) shared(camera) private(ray)
  for (int j = 0; j < camera.number_pixels[1]; j++) {
    for (int i = 0; i < camera.number_pixels[0]; i++) {
      ray = Ray(camera.position,
                camera.World_Position(ivec2(i, j)) - camera.position);

      camera.Set_Pixel(ivec2(i, j), Pixel_Color(Cast_Ray(ray, 1)));
    }
  }
}

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray &ray, int recursion_depth) {
  vec3 color;
  Hit closest = Closest_Intersection(ray);
  if (closest.object != nullptr) {
    vec3 point = ray.endpoint + closest.dist * ray.direction;
    return Shade_Surface(ray, point,
                         closest.object->Normal(point, closest.part),
                         recursion_depth, *this, closest.object->sd);
  } else {
    return Shade_Surface(ray, {0, 0, 0}, -ray.direction, recursion_depth, *this,
                         sd);
  }
}

void Render_World::Initialize_Hierarchy() {
  if (hierarchy_initialized) {
    return;
  }
  hierarchy_initialized = true;
  // Fill in hierarchy.entries; there should be one entry for
  // each part of each object.
  for (Object *object : objects) {
    for (int i = 0; i < object->number_parts; i++) {
      hierarchy.entries.push_back({object, i, object->Bounding_Box(i)});
    }
  }
  hierarchy.Reorder_Entries();
  hierarchy.Build_Tree();
}

void Render_World::Clear_Hierarchy() {
  hierarchy_initialized = false;

  hierarchy.entries.clear();
  hierarchy.tree.clear();
}

void Render_World::Update(double deltaT) {
  if (abs(deltaT) < std::numeric_limits<double>::epsilon()) {
    return;
  }
  camera.Update(deltaT);
  for (Object *object : objects) {
    object->Update(deltaT);
  }
  hierarchy.Update();
}

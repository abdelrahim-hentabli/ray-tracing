#include "shaders/phong_shader.hpp"

#include "lights/light.hpp"
#include "objects/object.hpp"
#include "ray.hpp"
#include "render_world.hpp"

vec3 Phong_Shader::Shade_Surface(const Ray &ray, const vec3 &intersection_point,
                                 const vec3 &normal,
                                 int recursion_depth) const {
  vec3 specular = {0, 0, 0};
  vec3 diffuse = {0, 0, 0};

  vec3 currentLightIntensity;
  vec3 currentLightDir;
  vec3 reflectionDir;
  vec3 normalizedLightDir;

  for (Light *light : world.lights) {
    currentLightDir = light->position - intersection_point;
    normalizedLightDir = currentLightDir.normalized();
    if (world.enable_shadows) {
      Hit closest = world.Closest_Intersection(
          Ray(intersection_point, normalizedLightDir));
      if (closest.object != nullptr) {
        if (closest.dist < currentLightDir.magnitude()) {
          continue;
        }
      }
    }
    if (dot(currentLightDir, normal) < 1e-6) {
      continue;
    } else {
      normalizedLightDir = currentLightDir.normalized();
      currentLightIntensity = light->Emitted_Light(currentLightDir);
      diffuse += currentLightIntensity *
                 std::max(0.0, dot(normal, normalizedLightDir));

      reflectionDir =
          2 * dot(normalizedLightDir, normal) * normal - normalizedLightDir;
      specular += currentLightIntensity *
                  std::pow(std::max(0.0, dot(reflectionDir, -ray.direction)),
                           specular_power);
    }
  }
  vec3 color = specular * color_specular +
               world.ambient_intensity * color_ambient * world.ambient_color +
               diffuse * color_diffuse;
  return color;
}

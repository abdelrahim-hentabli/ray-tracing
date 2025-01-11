#include "shaders/shader.hpp"

vec3 phong(const Ray &ray, const vec3 &intersection_point, const vec3 &normal,
           const Render_World &world, const vec3 &color_ambient,
           const vec3 &color_diffuse, const vec3 &color_specular,
           double specular_power) {
  vec3 specular = {0, 0, 0};
  vec3 diffuse = {0, 0, 0};

  vec3 currentLightIntensity;
  vec3 currentLightDir;
  vec3 reflectionDir;
  vec3 normalizedLightDir;
  for (auto light : world.lights) {
    currentLightDir = light->position - intersection_point;
    normalizedLightDir = currentLightDir.normalized();
    Hit closest =
        world.Closest_Intersection(Ray(intersection_point, normalizedLightDir));
    if (closest.object != nullptr) {
      if (closest.dist < currentLightDir.magnitude()) {
        continue;
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

vec3 Shade_Surface(shader_type type, const Ray &ray,
                   const vec3 &intersection_point, const vec3 &normal,
                   int recursion_depth, const Render_World &world,
                   const vec3 &color_ambient, const vec3 &color_diffuse,
                   const vec3 &color_specular, double specular_power,
                   double incidence_of_refraction, double color_intensity) {
  vec3 color;

  // Object Color
  switch (type) {
    case flat_shader:
      color = color_ambient;
      break;
    case reflective_flat:
    case refractive_flat:
      color = color_intensity * color_ambient;
      break;
    case phong_shader:
      color = phong(ray, intersection_point, normal, world, color_ambient,
                    color_diffuse, color_specular, specular_power);
      break;
    case reflective_phong:
    case refractive_phong:
      color = color_intensity * phong(ray, intersection_point, normal, world,
                                      color_ambient, color_diffuse,
                                      color_specular, specular_power);
      break;
    default:
      break;
  }

  //
  return color;
}

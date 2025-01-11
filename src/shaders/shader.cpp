#include "shaders/shader.hpp"

#include "lights/light.hpp"
#include "objects/object.hpp"
#include "render_world.hpp"

static inline vec3 reflect_vector(const vec3 &direction, const vec3 &normal) {
  return direction - 2 * dot(direction, normal) * normal;
}

static inline vec3 refract_vector(const vec3 &direction, const vec3 &normal,
                                  double incidence_of_refraction) {
  double cosi = std::max(-1., std::min(1., dot(direction, normal)));
  double etai = 1, etat = incidence_of_refraction;
  vec3 n = normal;
  if (cosi < 0) {
    cosi = -cosi;
  } else {
    std::swap(etai, etat);
    n = -n;
  }
  double eta = etai / etat;
  double k = 1 - eta * eta * (1 - cosi * cosi);
  return k < 0 ? vec3() : eta * direction + (eta * cosi - sqrtf(k)) * n;
}

static inline vec3 phong(const Ray &ray, const vec3 &intersection_point,
                         const vec3 &normal, const Render_World &world,
                         const vec3 &color_ambient, const vec3 &color_diffuse,
                         const vec3 &color_specular, double specular_power) {
  vec3 specular = {0, 0, 0};
  vec3 diffuse = {0, 0, 0};

  vec3 currentLightIntensity;
  vec3 currentLightDir;
  vec3 reflectionDir;
  vec3 normalizedLightDir;
  for (auto light : world.lights) {
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

static inline vec3 reflect(const Ray &ray, const vec3 &intersection_point,
                           const vec3 &normal, int recursion_depth,
                           const Render_World &world) {
  vec3 color = vec3(0, 0, 0);
  if (recursion_depth >= world.recursion_depth_limit) {
    return color;
  }
  vec3 reflectionDir = reflect_vector(ray.direction, normal);
  Hit closest =
      world.Closest_Intersection(Ray(intersection_point, reflectionDir));
  if (closest.object != nullptr) {
    vec3 point = intersection_point + closest.dist * reflectionDir;
    color = Shade_Surface(Ray(intersection_point, reflectionDir), point,
                          closest.object->Normal(point, closest.part),
                          recursion_depth + 1, world, closest.object->sd);
  } else {
    color = Shade_Surface(Ray(intersection_point, reflectionDir), {0, 0, 0},
                          {0, 0, 0}, 0, world, world.sd);
  }
  return color;
}

// calculates kr
static inline double fresnel(const vec3 &direction, const vec3 &normal,
                             double incidence_of_refraction) {
  float cosi = std::max(-1., std::min(1., dot(direction, normal)));
  float etai = 1, etat = incidence_of_refraction;
  if (cosi > 0) {
    std::swap(etai, etat);
  }
  // Compute sini using Snell's law
  float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
  // Total internal reflection
  if (sint >= 1) {
    return 1;
  } else {
    float cost = sqrtf(std::max(0.f, 1 - sint * sint));
    cosi = fabsf(cosi);
    float Rs =
        ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
    float Rp =
        ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
    return (Rs * Rs + Rp * Rp) / 2;
  }
}

static inline vec3 refract(const Ray &ray, const vec3 &intersection_point,
                           const vec3 &normal, int recursion_depth,
                           const Render_World &world, const vec3 &color_ambient,
                           const vec3 &color_diffuse,
                           const vec3 &color_specular, double specular_power,
                           double incidence_of_refraction) {
  vec3 color;
  if (recursion_depth >= world.recursion_depth_limit) {
    return color;
  }

  vec3 refractionColor = vec3();
  // compute fresnel
  double kr = fresnel(ray.direction, normal, incidence_of_refraction);
  bool outside = dot(ray.direction, normal) < 0;
  // compute refraction if it is not a case of total internal reflection
  if (kr < 1) {
    vec3 refractionDirection =
        refract_vector(ray.direction, normal, incidence_of_refraction)
            .normalized();
    vec3 refractionRayOrig =
        outside ? intersection_point - normal : intersection_point + normal;
    Hit closest =
        world.Closest_Intersection(Ray(refractionRayOrig, refractionDirection));
    if (closest.object != nullptr) {
      vec3 point = intersection_point + closest.dist * refractionDirection;
      color += (1 - kr) *
               Shade_Surface(Ray(intersection_point, refractionDirection),
                             point, closest.object->Normal(point, closest.part),
                             recursion_depth + 1, world, closest.object->sd);
    } else {
      color +=
          (1 - kr) * Shade_Surface(Ray(intersection_point, refractionDirection),
                                   {0, 0, 0}, {0, 0, 0}, 0, world, world.sd);
    }
  }

  vec3 reflectionDirection = reflect_vector(ray.direction, normal).normalized();
  vec3 reflectionRayOrig =
      outside ? intersection_point + normal : intersection_point - normal;
  Hit closest =
      world.Closest_Intersection(Ray(reflectionRayOrig, reflectionDirection));
  if (closest.object != nullptr) {
    vec3 point = intersection_point + closest.dist * reflectionDirection;
    color +=
        kr * Shade_Surface(Ray(intersection_point, reflectionDirection), point,
                           closest.object->Normal(point, closest.part),
                           recursion_depth + 1, world, closest.object->sd);
  } else {
    color += kr * Shade_Surface(Ray(intersection_point, reflectionDirection),
                                {0, 0, 0}, {0, 0, 0}, 0, world, world.sd);
  }

  return color;
}

vec3 Shade_Surface(const Ray &ray, const vec3 &intersection_point,
                   const vec3 &normal, int recursion_depth,
                   const Render_World &world, const shader_data &sd) {
  vec3 color;

  // Object Color
  switch (sd.type) {
    case flat_shader:
      color = sd.color_ambient;
      break;
    case reflective_flat:
      color =
          sd.color_intensity * sd.color_ambient +
          (1 - sd.color_intensity) *
              reflect(ray, intersection_point, normal, recursion_depth, world);
      break;
    case refractive_flat:
      color =
          sd.color_intensity * sd.color_ambient +
          (1 - sd.color_intensity) *
              refract(ray, intersection_point, normal, recursion_depth, world,
                      sd.color_ambient, sd.color_diffuse, sd.color_specular,
                      sd.specular_power, sd.incidence_of_refraction);
      break;
    case phong_shader:
      color = phong(ray, intersection_point, normal, world, sd.color_ambient,
                    sd.color_diffuse, sd.color_specular, sd.specular_power);
      break;
    case reflective_phong:
      color =
          sd.color_intensity * phong(ray, intersection_point, normal, world,
                                     sd.color_ambient, sd.color_diffuse,
                                     sd.color_specular, sd.specular_power) +
          (1 - sd.color_intensity) *
              reflect(ray, intersection_point, normal, recursion_depth, world);
      break;
    case refractive_phong:
      color =
          sd.color_intensity * phong(ray, intersection_point, normal, world,
                                     sd.color_ambient, sd.color_diffuse,
                                     sd.color_specular, sd.specular_power) +
          (1 - sd.color_intensity) *
              refract(ray, intersection_point, normal, recursion_depth, world,
                      sd.color_ambient, sd.color_diffuse, sd.color_specular,
                      sd.specular_power, sd.incidence_of_refraction);
      break;
    default:
      break;
  }

  return color;
}

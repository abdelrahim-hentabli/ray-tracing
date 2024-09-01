#include "shaders/refractive_shader.hpp"

#include "ray.hpp"
#include "render_world.hpp"

vec3 Refractive_Shader::Shade_Surface(const Ray &ray,
                                      const vec3 &intersection_point,
                                      const vec3 &normal,
                                      int recursion_depth) const {
  vec3 color = color_intensity * shader->Shade_Surface(ray, intersection_point,
                                                       normal, recursion_depth);
  if (recursion_depth >= world.recursion_depth_limit) {
    return color;
  }

  vec3 refractionColor = vec3();
  // compute fresnel
  double kr;
  fresnel(ray.direction, normal, kr);
  bool outside = dot(ray.direction, normal) < 0;
  // compute refraction if it is not a case of total internal reflection
  if (kr < 1) {
    vec3 refractionDirection = refract(ray.direction, normal).normalized();
    vec3 refractionRayOrig =
        outside ? intersection_point - normal : intersection_point + normal;
    Hit closest =
        world.Closest_Intersection(Ray(refractionRayOrig, refractionDirection));
    if (closest.object != nullptr) {
      vec3 point = intersection_point + closest.dist * refractionDirection;
      color +=
          (1 - color_intensity) * (1 - kr) *
          closest.object->material_shader->Shade_Surface(
              Ray(intersection_point, refractionDirection), point,
              closest.object->Normal(point, closest.part), recursion_depth + 1);
    } else {
      color += (1 - color_intensity) * (1 - kr) *
               world.background_shader->Shade_Surface(
                   Ray(intersection_point, refractionDirection), {0, 0, 0},
                   {0, 0, 0}, 0);
    }
  }

  vec3 reflectionDirection = reflect(ray.direction, normal).normalized();
  vec3 reflectionRayOrig =
      outside ? intersection_point + normal : intersection_point - normal;
  Hit closest =
      world.Closest_Intersection(Ray(reflectionRayOrig, reflectionDirection));
  if (closest.object != nullptr) {
    vec3 point = intersection_point + closest.dist * reflectionDirection;
    color +=
        (1 - color_intensity) * kr *
        closest.object->material_shader->Shade_Surface(
            Ray(intersection_point, reflectionDirection), point,
            closest.object->Normal(point, closest.part), recursion_depth + 1);
  } else {
    color += (1 - color_intensity) * kr *
             world.background_shader->Shade_Surface(
                 Ray(intersection_point, reflectionDirection), {0, 0, 0},
                 {0, 0, 0}, 0);
  }

  return color;
}

void Refractive_Shader::fresnel(const vec3 &direction, const vec3 &normal,
                                double &kr) const {
  float cosi = std::max(-1., std::min(1., dot(direction, normal)));
  float etai = 1, etat = incidence_of_refraction;
  if (cosi > 0) {
    std::swap(etai, etat);
  }
  // Compute sini using Snell's law
  float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
  // Total internal reflection
  if (sint >= 1) {
    kr = 1;
  } else {
    float cost = sqrtf(std::max(0.f, 1 - sint * sint));
    cosi = fabsf(cosi);
    float Rs =
        ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
    float Rp =
        ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
    kr = (Rs * Rs + Rp * Rp) / 2;
  }
  // As a consequence of the conservation of energy, the transmittance is given
  // by: kt = 1 - kr;
}

vec3 Refractive_Shader::refract(const vec3 &direction,
                                const vec3 &normal) const {
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

vec3 Refractive_Shader::reflect(const vec3 &direction,
                                const vec3 &normal) const {
  return direction - 2 * dot(direction, normal) * normal;
}

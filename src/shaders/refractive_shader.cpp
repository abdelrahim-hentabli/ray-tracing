#include "shaders/refractive_shader.hpp"

#include "ray.hpp"
#include "render_world.hpp"

vec3 Refractive_Shader::Shade_Surface(const Ray &ray,
                                      const vec3 &intersection_point,
                                      const vec3 &normal,
                                      int recursion_depth) const {
    vec3 color =  (color_intensity * shader->Shade_Surface(ray, intersection_point, normal, recursion_depth));

    if (recursion_depth >= world.recursion_depth_limit) {
        return color;
    }
    return color;
}

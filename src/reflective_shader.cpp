#include "reflective_shader.hpp"
#include "ray.hpp"
#include "render_world.hpp"

vec3 Reflective_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
    vec3 color;
    TODO; // determine the color
    return color;
}

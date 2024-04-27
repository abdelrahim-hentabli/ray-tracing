#include "shaders/flat_shader.hpp"

vec3 Flat_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
    return color;
}

#include "light.hpp"
#include "phong_shader.hpp"
#include "ray.hpp"
#include "render_world.hpp"
#include "object.hpp"

vec3 Phong_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
    vec3 color;
    TODO; //determine the color
    return color;
}

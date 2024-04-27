#include "shaders/reflective_shader.hpp"
#include "ray.hpp"
#include "render_world.hpp"

vec3 Reflective_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
    vec3 color = (1-reflectivity) * shader->Shade_Surface(ray, intersection_point, normal, recursion_depth);;
    if(recursion_depth >= world.recursion_depth_limit){
        return color;
    }
    vec3 reflectionDir = ray.direction - 2 * dot(ray.direction, normal) * normal ;
    Hit closest = world.Closest_Intersection(Ray(intersection_point,reflectionDir));
    if (closest.object != nullptr){
        vec3 point = intersection_point + closest.dist * reflectionDir;
        color += reflectivity * closest.object->material_shader->Shade_Surface(Ray(intersection_point,reflectionDir),point,closest.object->Normal(point, closest.part),recursion_depth+1);
    }
    else {
        color += reflectivity * world.background_shader->Shade_Surface(Ray(intersection_point,reflectionDir),{0,0,0},{0,0,0},0);
    }
    return color;
}

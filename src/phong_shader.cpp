#include "light.hpp"
#include "phong_shader.hpp"
#include "ray.hpp"
#include "render_world.hpp"
#include "object.hpp"

vec3 Phong_Shader::
Shade_Surface(const Ray& ray, const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
    vec3 specular = {0,0,0};
    vec3 diffuse = {0,0,0};

    vec3 currentLightIntensity;
    vec3 currentLightDir;
    for (Light *light: world.lights){
        currentLightDir = light->position - intersection_point;
        if (dot(currentLightDir, normal) < 1e-6) {
            continue;
        }
        else {
            currentLightIntensity = light->Emitted_Light(currentLightDir);
            diffuse += currentLightIntensity * std::max(0.0, dot(normal,currentLightDir.normalized()));
        }
    }    
    vec3 color = specular + world.ambient_intensity * color_ambient * world.ambient_color + diffuse * color_diffuse;
    return color;
}

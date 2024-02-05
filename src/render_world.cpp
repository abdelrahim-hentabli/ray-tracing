#include <limits>

#include "render_world.hpp"
#include "flat_shader.hpp"
#include "object.hpp"
#include "light.hpp"
#include "ray.hpp"

Render_World::Render_World()
    :background_shader(0),ambient_intensity(0),enable_shadows(true),
    recursion_depth_limit(3)
{}

Render_World::~Render_World()
{
    delete background_shader;
    for(size_t i=0;i<objects.size();i++) delete objects[i];
    for(size_t i=0;i<lights.size();i++) delete lights[i];
}

// Find and return the Hit structure for the closest intersection.  Be careful
// to ensure that hit.dist>=small_t.
Hit Render_World::Closest_Intersection(const Ray& ray)
{
    Hit temp;
    Hit output {nullptr, std::numeric_limits<double>::max(), 0};
    std::vector<int> candidates;
    hierarchy.Intersection_Candidates(ray, candidates);
    for (int candidate : candidates){
        temp = hierarchy.entries[candidate].obj->Intersection(ray,hierarchy.entries[candidate].part);
        if (temp.object != nullptr) {
            if(temp.dist < output.dist){
                output = temp;
            }
        }
    }
    return output;
}


// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{
    Ray ray = Ray(camera.position, camera.World_Position(pixel_index) - camera.position);
    vec3 color=Cast_Ray(ray,1);
    camera.Set_Pixel(pixel_index,Pixel_Color(color));
}

void Render_World::Render()
{
    Initialize_Hierarchy();

    for(int j=0;j<camera.number_pixels[1];j++)
        for(int i=0;i<camera.number_pixels[0];i++)
            Render_Pixel(ivec2(i,j));
}

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray& ray,int recursion_depth)
{ 
    vec3 color;
    Hit closest = Closest_Intersection(ray);
    if (closest.object != nullptr){
        vec3 point = ray.endpoint + closest.dist * ray.direction;
        return closest.object->material_shader->Shade_Surface(ray, point, closest.object->Normal(point, closest.part), recursion_depth);
    }
    else{
        return background_shader->Shade_Surface(ray, {0,0,0}, -ray.direction, recursion_depth);
    }
}

void Render_World::Initialize_Hierarchy()
{
    // Fill in hierarchy.entries; there should be one entry for
    // each part of each object.
    for(Object* object: objects){
        for(int i = 0; i < object->number_parts; i++){
            hierarchy.entries.push_back({object,i,object->Bounding_Box(i)});
        }
    }
    hierarchy.Reorder_Entries();
    hierarchy.Build_Tree();
}

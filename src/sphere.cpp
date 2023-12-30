#include "sphere.h"
#include "ray.h"

// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
    TODO;
    vec3 L = center - ray.endpoint;
    double tc = dot(L, ray.direction);
    if (tc < 0.0){
        return {0,0,0};
    }
    float dist_to_center_squared = (tc * tc) - L.magnitude_squared();
    if(abs(dist_to_center_squared) >  radius * radius){
        return {0,0,0};
    }

    float t1c = sqrt(radius * radius - dist_to_center_squared);
    
    return {this,tc - t1c,0};
}

vec3 Sphere::Normal(const vec3& point, int part) const
{
    vec3 normal;
    TODO; // compute the normal direction
    return normal;
}

Box Sphere::Bounding_Box(int part) const
{
    Box box;
    TODO; // calculate bounding box
    return box;
}

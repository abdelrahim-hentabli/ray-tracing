#include "objects/sphere.hpp"
#include "ray.hpp"

// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
    vec3 L = center - ray.endpoint;
    double tc = dot(L, ray.direction);
    if (tc < 0.0){
        return {0,0,0};
    }
    float dist_to_center_squared = L.magnitude_squared() - tc * tc;
    if(abs(dist_to_center_squared) >  radius * radius){
        return {0,0,0};
    }

    float t1c = sqrt(radius * radius - dist_to_center_squared);

    return {this,tc - t1c,0};
}

vec3 Sphere::Normal(const vec3& point, int part) const
{
    vec3 normal = point - center;
    return normal.normalized();
}

Box Sphere::Bounding_Box(int part) const
{
    Box box;
    vec3 radius_vector = {radius, radius, radius};
    box.hi = center + radius_vector;
    box.lo = center - radius_vector;
    return box;
}

#include <limits>
#include "box.hpp"

// Return whether the ray intersects this box.
bool Box::Intersection(const Ray& ray) const
{
    double tmin = (lo[0] - ray.endpoint[0]) / ray.direction[0];
    double tmax = (hi[0] - ray.endpoint[0]) / ray.direction[0];
    
    if (tmin > tmax) {
        std::swap(tmin, tmax); 
    }

    double tymin = (lo[1] - ray.endpoint[1]) / ray.direction[1];
    double tymax = (hi[1] - ray.endpoint[1]) / ray.direction[1];
    
    if (tymin > tymax) {
        std::swap(tymin, tymax); 
    }
    if ((tmin > tymax) || (tymin > tmax)) {
        return false; 
    }
    
    tmin = std::max(tmin,tymin);
    tmax = std::min(tmax,tymax);
    
    double tzmin = (lo[2] - ray.endpoint[2]) / ray.direction[2];
    double tzmax = (hi[2] - ray.endpoint[2]) / ray.direction[2];

    if (tzmin > tzmax) {
        std::swap(tzmin, tzmax);
    }

    if ((tmin > tzmax) || (tzmin > tmax)) {
        return false; 
    }
    
    return true;
}

// Compute the smallest box that contains both *this and bb.
Box Box::Union(const Box& bb) const
{
    Box box;
    for(int axis = 0; axis < 3; axis++){
        box.hi[axis] = std::max(hi[axis], bb.hi[axis]);
        box.lo[axis] = std::min(lo[axis], bb.lo[axis]);
    }
    return box;
}

// Enlarge this box (if necessary) so that pt also lies inside it.
void Box::Include_Point(const vec3& pt)
{
    for(int axis = 0; axis < 3; axis++){
        hi[axis] = std::max(hi[axis], pt[axis]);
        lo[axis] = std::min(lo[axis], pt[axis]);
    }
}

// Create a box to which points can be correctly added using Include_Point.
void Box::Make_Empty()
{
    lo.fill(std::numeric_limits<double>::infinity());
    hi=-lo;
}

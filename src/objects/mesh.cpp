#include "objects/mesh.hpp"

#include <fstream>
#include <limits>
#include <string>

#include "directories.hpp"

// Consider a triangle to intersect a ray if the ray intersects the plane of the
// triangle with barycentric weights in [-weight_tolerance, 1+weight_tolerance]
static const double weight_tolerance = 1e-4;

// Read in a mesh from an obj file.  Populates the bounding box and registers
// one part per triangle (by setting number_parts).
void Mesh::Read_Obj(const char *file) {
  std::ifstream fin(getObjectsDir() + file);
  if (!fin) {
    exit(EXIT_FAILURE);
  }
  std::string line;
  ivec3 e;
  vec3 v;
  box.Make_Empty();
  while (fin) {
    getline(fin, line);

    if (sscanf(line.c_str(), "v %lg %lg %lg", &v[0], &v[1], &v[2]) == 3) {
      vertices.push_back(v);
      box.Include_Point(v);
    }

    if (sscanf(line.c_str(), "f %d %d %d", &e[0], &e[1], &e[2]) == 3) {
      for (int i = 0; i < 3; i++) e[i]--;
      triangles.push_back(e);
    }
  }
  number_parts = triangles.size();
}

// Check for an intersection against the ray.  See the base class for details.
Hit Mesh::Intersection(const Ray &ray, int part) const {
  double dist;
  if (part >= 0) {
    if (part >= number_parts) {
      return {};
    }
    if (!Bounding_Box(part).Intersection(ray)) {
      return {};
    }
    if (Intersect_Triangle(ray, part, dist)) {
      return {this, dist, part};
    }
  } else {
    if (!box.Intersection(ray)) {
      return {};
    }
    int min_part = -1;
    double min_dist = std::numeric_limits<double>::infinity();
    for (int i = 0; i < number_parts; i++) {
      if (Intersect_Triangle(ray, i, dist)) {
        if (dist < min_dist) {
          min_dist = dist;
          min_part = i;
        }
      }
    }
    if (min_part == -1) {
      return {};
    } else {
      return {this, min_dist, min_part};
    }
  }
  return {};
}

// Compute the normal direction for the triangle with index part.
vec3 Mesh::Normal(const vec3 &point, int part) const {
  assert(part >= 0);
  vec3 point0 = vertices[triangles[part][0]];
  vec3 point1 = vertices[triangles[part][1]];
  vec3 point2 = vertices[triangles[part][2]];

  vec3 normal = cross(point1 - point0, point2 - point0).normalized();
  return normal;
}

// This is a helper routine whose purpose is to simplify the implementation
// of the Intersection routine.  It should test for an intersection between
// the ray and the triangle with index tri.  If an intersection exists,
// record the distance and return true.  Otherwise, return false.
// This intersection should be computed by determining the intersection of
// the ray and the plane of the triangle.  From this, determine (1) where
// along the ray the intersection point occurs (dist) and (2) the barycentric
// coordinates within the triangle where the intersection occurs.  The
// triangle intersects the ray if dist>small_t and the barycentric weights are
// larger than -weight_tolerance.  The use of small_t avoid the self-shadowing
// bug, and the use of weight_tolerance prevents rays from passing in between
// two triangles.
bool Mesh::Intersect_Triangle(const Ray &ray, int tri, double &dist) const {
  ivec3 triangle = triangles[tri];
  vec3 point0 = vertices[triangle[0]];
  vec3 point1 = vertices[triangle[1]];
  vec3 point2 = vertices[triangle[2]];

  vec3 normal = cross(point1 - point0, point2 - point0);

  double bespoke_weight_tolerance = normal.magnitude_squared() * -weight_tol;

  if (abs(dot(normal, ray.direction)) < -1e-4) {
    return false;
  }

  double d = -dot(normal, point0);

  double t = -(dot(normal, ray.endpoint) + d) / dot(normal, ray.direction);
  if (t < small_t) {
    return false;
  }

  vec3 p = ray.endpoint + t * ray.direction;

  vec3 c = cross(point1 - point0, p - point0);
  if (dot(normal, c) < bespoke_weight_tolerance) {
    return false;
  }

  c = cross(point2 - point1, p - point1);
  if (dot(normal, c) < bespoke_weight_tolerance) {
    return false;
  }
  c = cross(point0 - point2, p - point2);
  if (dot(normal, c) < bespoke_weight_tolerance) {
    return false;
  }

  dist = t;

  return true;
}

// Compute the bounding box.  Return the bounding box of only the triangle whose
// index is part.
Box Mesh::Bounding_Box(int part) const {
  Box b;
  b.Make_Empty();
  b.Include_Point(vertices[triangles[part][0]]);
  b.Include_Point(vertices[triangles[part][1]]);
  b.Include_Point(vertices[triangles[part][2]]);
  return b;
}

void Mesh::Update(double deltaT) {}

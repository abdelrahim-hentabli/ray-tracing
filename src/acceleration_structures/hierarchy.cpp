#include "acceleration_structures/hierarchy.hpp"

#include <algorithm>
#include <numeric>
#include <queue>

// Reorder the entries vector so that adjacent entries tend to be nearby.
// You may want to implement box.cpp first.
void Hierarchy::Reorder_Entries() {
  if (!entries.size()) return;

  vec3 sum;
  for (Entry entry : entries) {
    sum += entry.box.Get_Center();
  }

  vec3 mean = sum / entries.size();

  int furthest_index = 0;
  double distance_squared_to_center =
      (mean - entries[0].box.Get_Center()).magnitude_squared();
  for (int i = 1; i < entries.size(); i++) {
    if ((mean - entries[i].box.Get_Center()).magnitude_squared() >
        distance_squared_to_center) {
      furthest_index = i;
      distance_squared_to_center =
          (mean - entries[i].box.Get_Center()).magnitude_squared();
    }
  }

  std::swap(entries[0], entries[furthest_index]);

  int nearest_index;
  double nearest_distance;
  for (int i = 0; i < (int)entries.size() - 2; i++) {
    nearest_index = i + 1;
    nearest_distance =
        (entries[i].box.Get_Center() - entries[nearest_index].box.Get_Center())
            .magnitude_squared();
    for (int j = i + 2; j < entries.size(); j++) {
      if ((entries[i].box.Get_Center() - entries[j].box.Get_Center())
              .magnitude_squared() < nearest_distance) {
        nearest_index = j;
        nearest_distance =
            (entries[i].box.Get_Center() - entries[j].box.Get_Center())
                .magnitude_squared();
      }
    }
    std::swap(entries[i + 1], entries[nearest_index]);
  }
}

// Populate tree from entries.
void Hierarchy::Build_Tree() {
  int n = entries.size();
  if (!n) return;
  tree = std::vector<Box>(2 * n - 1);
  for (int i = 0; i < n; i++) {
    tree[n - 1 + i] = entries[i].box;
  }
  for (int i = n - 2; i >= 0; i--) {
    tree[i] = tree[2 * i + 1].Union(tree[2 * i + 2]);
  }
}

// Return a list of candidates (indices into the entries list) whose
// bounding boxes intersect the ray.
void Hierarchy::Intersection_Candidates(const Ray &ray,
                                        std::vector<int> &candidates) const {
  int n = entries.size();
  if (!n) return;
  std::queue<int> check_nodes;
  check_nodes.push(0);
  int temp;
  while (!check_nodes.empty()) {
    temp = check_nodes.front();
    if (tree[temp].Intersection(ray)) {
      if (temp >= n - 1) {
        candidates.push_back(temp - (n - 1));
      } else {
        check_nodes.push(temp * 2 + 1);
        check_nodes.push(temp * 2 + 2);
      }
    }
    check_nodes.pop();
  }
}

void Hierarchy::Update() {}

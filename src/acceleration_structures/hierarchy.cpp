#include "acceleration_structures/hierarchy.hpp"

#include <algorithm>
#include <numeric>
#include <queue>

// Reorder the entries vector so that adjacent entries tend to be nearby.
// You may want to implement box.cpp first.
void Hierarchy::Reorder_Entries() {
  if (!entries.size()) return;
  // sort based on bounding box lo, kinda stupid. But a start
  std::sort(entries.begin(), entries.end(), [](Entry first, Entry second) {
    return first.box.lo[1] < second.box.lo[1];
  });
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

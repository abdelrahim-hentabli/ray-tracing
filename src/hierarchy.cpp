#include <algorithm>
#include <numeric>
#include <queue>
#include "hierarchy.hpp"

// Reorder the entries vector so that adjacent entries tend to be nearby.
// You may want to implement box.cpp first.
void Hierarchy::Reorder_Entries()
{
    if(!entries.size()) return;
    // sort based on bounding box lo, kinda stupid. But a start
    std::sort(entries.begin(), entries.end(), [](Entry first, Entry second) {return first.box.lo.magnitude_squared() < second.box.lo.magnitude_squared();});
    TODO;
}

// Populate tree from entries.
void Hierarchy::Build_Tree()
{
    int n = entries.size();
    if(!n) return;
    if(n == 1){
        tree.push_back(entries[0].box);
    }
    tree = std::vector<Box>(2 * n -1);
    for (int i = 0 ; i < n; i++) {
        tree[ n-1 + i] = entries[i].box;
    }
    for(int i = n-2; i >=0; i--){
        tree[i] = tree[2*i].Union(tree[2*i+1]);
    }
    TODO;
}

// Return a list of candidates (indices into the entries list) whose
// bounding boxes intersect the ray.
void Hierarchy::Intersection_Candidates(const Ray& ray, std::vector<int>& candidates) const
{
    int n = entries.size();
    if(!n) return;
    std::queue<int> check_nodes;
    check_nodes.push(0);
    while(!check_nodes.empty()){
        if(tree[check_nodes.front()].Intersection(ray)){
            if (check_nodes.front() >= n-1) {
                candidates.push_back(check_nodes.front() - (n-1));
            }
            else {
                check_nodes.push(check_nodes.front() * 2);
                check_nodes.push(check_nodes.front() * 2 + 1);
            }
        }
        check_nodes.pop();
    }
}

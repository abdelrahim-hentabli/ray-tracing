#ifndef __BENCHMARK_HIERARCHY_HPP__
#define __BENCHMARK_HIERARCHY_HPP__

#include <benchmark/benchmark.h>
#include "hierarchy.hpp"
#include "directories.hpp"
#include "render_world.hpp"
#include "parse.hpp"

static void BM_setupHierarchy(benchmark::State& state) {
    int width=0;
    int height=0;
    Render_World world;
    
    // Parse test scene file
    Parse(world,width,height,(getCasesDir()  + "28.txt").c_str());
    
    for (auto _ : state) {
        world.Initialize_Hierarchy();
    }
}

BENCHMARK(BM_setupHierarchy);

class IntersectionCandidatesFixture : public benchmark::Fixture {
public:
    void SetUp(::benchmark::State& state) {
       
        // Parse test scene file
        Parse(world,width,height,(getCasesDir()  + "28.txt").c_str());
        world.Initialize_Hierarchy();
            
        Camera tempCamera;
        vec3 look = {0,0,0};
        vec3 up = {0,1,0};
        cardinal_directions = std::vector<vec3>(6);
        direction_names = std::vector<std::string>(6);
    
        cardinal_directions[0] = {0,0,1};
        direction_names[0] = "north";
        
        cardinal_directions[1] = {1,0,0};
        direction_names[1] = "east";
        
        cardinal_directions[2] = {0,0,-1};
        direction_names[2] = "south";
        
        cardinal_directions[3] = {-1,0,0};
        direction_names[3] = "west";
        
        cardinal_directions[4] = {0,1,0};
        direction_names[4] = "up";

        cardinal_directions[5] = {0,-1,0};
        direction_names[5] = "down";
        
        for(auto direction: cardinal_directions){

            tempCamera = Camera();
            tempCamera.Position_And_Aim_Camera(direction, look, up);
            tempCamera.Focus_Camera(1,(double)width/height,f0*(pi/180));
            tempCamera.Set_Resolution(ivec2(width,height));
            nominal_direction_cameras.push_back(tempCamera);
        }

    }

  void TearDown(::benchmark::State& state) {
  }

public:
    std::vector<vec3>   cardinal_directions;
    std::vector<std::string> direction_names;
    int width=0;
    int height=0;
    Render_World world;
    std::vector<Camera> nominal_direction_cameras;
    double f0 = 70;
};

BENCHMARK_DEFINE_F(IntersectionCandidatesFixture, NominalDirections)(benchmark::State& state) {
    Ray ray;
    int current_camera_index = state.range(0);
    state.SetLabel(direction_names[current_camera_index]);
    world.camera = nominal_direction_cameras[current_camera_index];

    for (auto _ : state) {
        for(int j=0;j<world.camera.number_pixels[1];j++) {
            for(int i=0;i<world.camera.number_pixels[0];i++) {
                ray = Ray(world.camera.position, world.camera.World_Position({i,j}) - world.camera.position);
                world.Closest_Intersection(ray);
            }
        }
    }
}

BENCHMARK_REGISTER_F(IntersectionCandidatesFixture, NominalDirections)->Name("IntersectionCandidatesByDirection")->DenseRange(0,5,1);

#endif // __BENCHMARK_HIERARCHY_HPP__
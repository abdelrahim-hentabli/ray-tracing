#ifndef __BENCHMARK_HIERARCHY_HPP__
#define __BENCHMARK_HIERARCHY_HPP__

#include <benchmark/benchmark.h>
#include "hierarchy.hpp"
#include "directories.hpp"
#include "render_world.hpp"
#include "parse.hpp"
#include "point_light.hpp"
#include "mesh.hpp"
#include "flat_shader.hpp"
#include "phong_shader.hpp"


Render_World SetupBenchmarkWorld(int width, int height, vec3 cameraP, vec3 cameraL, vec3 cameraU) {
    Render_World world;

    // Setup world
    world.enable_shadows  = false;
    world.recursion_depth_limit = 1;
    world.background_shader=new Flat_Shader(world,vec3());

    // Setup objects
    Mesh* o=new Mesh;
    o->Read_Obj("sphere.obj");
    o->material_shader = new Phong_Shader(world, {1,1,1}, {1,1,1}, {1,1,1}, 50);
    world.objects.push_back(o);

    // Setup lights
    world.lights.push_back(new Point_Light(vec3(.8, .8, 4), vec3(1,1,1), 100));
    world.ambient_color     = {1,1,1};
    world.ambient_intensity = 0;
    
    // Setup Camera
    world.camera.Position_And_Aim_Camera(cameraP,cameraL,cameraU);
    world.camera.Focus_Camera(1,(double)width/height,70*(pi/180));
    
    world.camera.Set_Resolution(ivec2(width,height));
    
    return world;
}

static void BM_setupHierarchy(benchmark::State& state) {
    int width=0;
    int height=0;
    Render_World world = std::move(SetupBenchmarkWorld(640,480, vec3(0,0,1), vec3(0,0,0), vec3(0,1,0)));
    
    for (auto _ : state) {
        world.Initialize_Hierarchy();
        world.Clear_Hierarchy();
    }
}

BENCHMARK(BM_setupHierarchy);

class IntersectionCandidatesFixture : public benchmark::Fixture {
public:
    void SetUp(::benchmark::State& state) {
        Camera tempCamera;
        vec3 look = {0,0,0};
        vec3 up = {0,1,0};

        // Parse test scene file
        world = std::move(SetupBenchmarkWorld(width,height, vec3(0,0,2), look, up));
        world.Initialize_Hierarchy();        
        
        cardinal_directions = std::vector<vec3>(6);
        direction_names = std::vector<std::string>(6);
    
        cardinal_directions[0] = {0,0,2};
        direction_names[0] = "north";
        
        cardinal_directions[1] = {2,0,0};
        direction_names[1] = "east";
        
        cardinal_directions[2] = {0,0,-2};
        direction_names[2] = "south";
        
        cardinal_directions[3] = {-2,0,0};
        direction_names[3] = "west";
        
        cardinal_directions[4] = {0,2,0};
        direction_names[4] = "up";

        cardinal_directions[5] = {0,-2,0};
        direction_names[5] = "down";
        
        for(int i = 0; i < cardinal_directions.size(); i++){
            vec3 direction = cardinal_directions[i];
            if (i > 3){
                look = {0,0,1};
            }
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
    int width=640;
    int height=480;
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
cmake_minimum_required(VERSION 3.16)

set (CMAKE_CXX_STANDARD 11)

project(ray_tracer)
add_executable(ray_tracer main.cpp camera.cpp hierarchy.cpp dump_png.cpp flat_shader.cpp parse.cpp phong_shader.cpp plane.cpp reflective_shader.cpp render_world.cpp sphere.cpp box.cpp mesh.cpp)
find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

target_link_libraries(ray_tracer ${PNG_LIBRARY})
add_definitions(-std=c++11)
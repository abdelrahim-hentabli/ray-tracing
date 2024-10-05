cmake_minimum_required(VERSION 3.16)

set (CMAKE_CXX_STANDARD 14)
project(ray_tracer)

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

option(USE_OMP "Use OpenMP for parallelization" OFF)

if (USE_OMP)
    find_package(OpenMP REQUIRED)
endif()

add_subdirectory(src)
add_subdirectory(tools)

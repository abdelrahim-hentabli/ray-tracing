file(GLOB RAY_TRACER_SOURCES
    acceleration_structures/*.cpp
    objects/*.cpp
    shaders/*.cpp
    *.cpp)

add_library(ray_tracer SHARED ${RAY_TRACER_SOURCES})

target_include_directories(ray_tracer
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_BINARY_DIR}/generated)

target_compile_options(ray_tracer
    PUBLIC -ffast-math)

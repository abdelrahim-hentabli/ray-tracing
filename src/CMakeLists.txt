add_executable(ray_tracer main.cpp camera.cpp hierarchy.cpp dump_png.cpp flat_shader.cpp parse.cpp phong_shader.cpp plane.cpp reflective_shader.cpp render_world.cpp sphere.cpp box.cpp mesh.cpp)
find_package(PNG REQUIRED)

target_include_directories(ray_tracer
    PRIVATE ${PNG_INCLUDE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(ray_tracer ${PNG_LIBRARY})

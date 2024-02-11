add_executable(benchmarks main.cpp)

target_include_directories(benchmarks
    PRIVATE ${PROJECT_SOURCE_DIR}/include)
    
target_link_libraries(benchmarks
    ray_tracer
    benchmark::benchmark)

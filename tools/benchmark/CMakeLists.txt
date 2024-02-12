add_executable(benchmarks source/main.cpp)

target_include_directories(benchmarks
    PRIVATE ${PROJECT_SOURCE_DIR}/include
    PRIVATE ./include
    PRIVATE ${CMAKE_BINARY_DIR}/generated)
    
target_link_libraries(benchmarks
    ray_tracer
    benchmark::benchmark)

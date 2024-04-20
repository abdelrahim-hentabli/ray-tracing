add_executable(benchmarks source/main.cpp)

target_include_directories(benchmarks
    PRIVATE ${PROJECT_SOURCE_DIR}/include
    PRIVATE ./include
    PRIVATE ${CMAKE_BINARY_DIR}/generated
    PRIVATE $<TARGET_PROPERTY:tool_common,INTERFACE_INCLUDE_DIRECTORIES>)
    
target_link_libraries(benchmarks
    PRIVATE ray_tracer
    PRIVATE tool_common
    PRIVATE benchmark::benchmark)

add_executable(tests source/main.cpp)
find_package(PNG REQUIRED)

add_subdirectory(cases)
add_subdirectory(objects)
add_subdirectory(outputs)
add_subdirectory(configs)

target_include_directories(tests
    PRIVATE ${PNG_INCLUDE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/include
    PRIVATE ./include
    PRIVATE ${CMAKE_BINARY_DIR}/generated
    PRIVATE $<TARGET_PROPERTY:tool_common,INTERFACE_INCLUDE_DIRECTORIES>)

target_link_libraries(tests 
    ray_tracer
    tool_common
    ${PNG_LIBRARY}
    GTest::gtest_main)
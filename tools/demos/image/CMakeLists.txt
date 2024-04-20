add_executable(image_demo main.cpp)

target_include_directories(image_demo
    PRIVATE ${PROJECT_SOURCE_DIR}/include
    PRIVATE $<TARGET_PROPERTY:tool_common,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(image_demo
    PRIVATE ray_tracer
    PRIVATE tool_common
)
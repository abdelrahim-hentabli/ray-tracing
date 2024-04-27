find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
    libavutil
    libavcodec
    libavformat
    libswscale
    libswresample)

add_executable(video_demo main.cpp)

target_include_directories(video_demo
    PRIVATE ${PROJECT_SOURCE_DIR}/include
    PRIVATE $<TARGET_PROPERTY:tool_common,INTERFACE_INCLUDE_DIRECTORIES>)

target_link_libraries(video_demo
    # PRIVATE ray_tracer
    PRIVATE tool_common)

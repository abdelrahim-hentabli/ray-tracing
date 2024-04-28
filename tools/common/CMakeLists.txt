set (CMAKE_CXX_STANDARD 11)

find_package(PNG REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
    libavutil
    libavcodec
    libavformat
    libswscale
    libswresample
)

file(GLOB TOOL_COMMON_SOURCES *.cpp)

add_library(tool_common OBJECT ${TOOL_COMMON_SOURCES})

set_target_properties(tool_common PROPERTIES CXX_STANDARD 17)

target_include_directories(tool_common
    PUBLIC  .
    PRIVATE ${PNG_INCLUDE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(tool_common
    PRIVATE ray_tracer
    PRIVATE ${PNG_LIBRARY}
    PRIVATE PkgConfig::LIBAV)

find_package(PNG REQUIRED)

file(GLOB TOOL_COMMON_SOURCES *.cpp)

add_library(tool_common OBJECT ${TOOL_COMMON_SOURCES})

set_target_properties(tool_common PROPERTIES CXX_STANDARD 17)

target_include_directories(tool_common
    PUBLIC  .
    PRIVATE ${PNG_INCLUDE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_BINARY_DIR}/generated
)

target_link_libraries(tool_common
    PUBLIC ${PNG_LIBRARY})

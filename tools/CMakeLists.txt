set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(third-party/googletest)
add_subdirectory(third-party/benchmark)
add_subdirectory(common)
add_subdirectory(demos)
add_subdirectory(test)
add_subdirectory(benchmark)
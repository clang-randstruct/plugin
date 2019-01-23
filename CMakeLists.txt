cmake_minimum_required(VERSION 3.12)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
find_package(Clang REQUIRED)

add_subdirectory(src)

cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
find_package(Clang REQUIRED)

add_library(randstruct SHARED randstruct.cpp)

target_include_directories(randstruct PUBLIC ${CLANG_INCLUDE_DIRS})
set_target_properties(randstruct PROPERTIES
    LINKER_LANGUAGE CXX)

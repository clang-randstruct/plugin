find_package(Clang REQUIRED)
add_library(randstruct SHARED plugin.cpp randstruct.cpp)
target_include_directories(randstruct PUBLIC ${CLANG_INCLUDE_DIRS})

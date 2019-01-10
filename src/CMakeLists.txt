find_package(Clang REQUIRED)
add_library(randstruct SHARED randstruct.cpp)
target_include_directories(randstruct PUBLIC ${CLANG_INCLUDE_DIRS})

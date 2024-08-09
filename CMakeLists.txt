cmake_minimum_required(VERSION 3.12)

project(Puma)
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(tests)
cmake_minimum_required(VERSION 3.0)
project(myComp)

set(CMAKE_CXX_STANDARD 20)

# set the output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
# the directory of the header files
include_directories(./include)
# the directory of the source files
aux_source_directory(./src SRC_DIR)
# add the source files to the executable
add_executable(myComp ${SRC_DIR} main.cpp)

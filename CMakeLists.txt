cmake_minimum_required(VERSION 3.5.1)
project (DependencySolver)

set (CMAKE_CXX_STANDARD 11)

find_package(Threads)
include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(DepSolver ${SOURCES})

target_link_libraries (DepSolver ${CMAKE_THREAD_LIBS_INIT})
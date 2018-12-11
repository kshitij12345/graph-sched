cmake_minimum_required(VERSION 3.5.1)
project (DependencySolver)

set (CMAKE_CXX_STANDARD 11)

find_package(Threads)
find_package(CUDA)
include_directories(include)


# Library
file(GLOB LIB_SRCS "lib/*.cpp")
add_library(gsched ${LIB_SRCS})

# CPU Example
file(GLOB SOURCES "examples/*.cpp")
add_executable(DepSolver ${SOURCES} ${LIB_SRCS})
target_link_libraries (DepSolver gsched ${CMAKE_THREAD_LIBS_INIT})

if(CUDA_FOUND)
# GPU Example
file(GLOB CUDA_SOURCES "examples/cuda_example/*.cu")
list(APPEND CUDA_NVCC_FLAGS " -std=c++11;")
cuda_add_executable(DepSolverCUDA ${CUDA_SOURCES})
target_link_libraries (DepSolverCUDA gsched ${CMAKE_THREAD_LIBS_INIT})

endif(CUDA_FOUND)





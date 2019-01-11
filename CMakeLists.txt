cmake_minimum_required(VERSION 3.5.1)
project (DependencySolver)

set (CMAKE_CXX_STANDARD 11)

find_package(Threads)
find_package(CUDA)
include_directories(include)
include_directories(third_party)


# Library
file(GLOB LIB_SRCS "lib/*.cpp")
add_compile_options(-fsanitize=thread)
add_library(gsched ${LIB_SRCS})

# CPU Example
file(GLOB SOURCES "test/*.cpp")
add_compile_options(-fsanitize=thread)
add_executable(test_cpu ${SOURCES} ${LIB_SRCS})
target_link_libraries (test_cpu gsched ${CMAKE_THREAD_LIBS_INIT} -fsanitize=thread)

#if(CUDA_FOUND)
# GPU Example
#file(GLOB CUDA_SOURCES "test/*.cu")
#list(APPEND CUDA_NVCC_FLAGS " -std=c++11;")
#cuda_add_executable(test_cuda ${CUDA_SOURCES})
#target_link_libraries (test_cuda gsched ${CMAKE_THREAD_LIBS_INIT})

#endif(CUDA_FOUND)





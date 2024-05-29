#cmake_minimum_required(VERSION 2.8.0)
#project(llama)

set(CMAKE_CXX_STANDARD 11)
if(CMAKE_COMPILER_IS_GNUCXX)
    SET (CMAKE_CXX_FLAGS "-std=gnu++11 -fopenmp ${CXX_FLAGS_Common} ")
    message(STATUS "optional:-std=gnu++11")
endif(CMAKE_COMPILER_IS_GNUCXX)

## install the header file to outside
#add_subdirectory(llama/include)
## cmake -DCMAKE_INSTALL_PREFIX=/usr ..

include_directories(include)
include_directories(tests/benchmark)
include_directories(include/utils)
include_directories(include/algorithms)

add_executable(llama_edge tests/llama_test_edge.cpp)
add_executable(llama_propotion tests/llama_test_propotion.cpp)
add_executable(llama_alg tests/llama_test_alg.cpp)
add_executable(llama_mem tests/llama_test_memory.cpp)
add_executable(llama_scala tests/llama_test_scala.cpp)
#add_executable(llama_test tests/llama_test.cpp)

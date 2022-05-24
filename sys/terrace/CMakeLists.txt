cmake_minimum_required(VERSION 3.0)
project(terrace)

#set(CMAKE_CXX_COMPILER /home/zxy/Desktop/Code/cilk/OpenCilk-12.0.0-Linux/bin/clang++)
set(CMAKE_CXX_STANDARD 17)
if(CMAKE_COMPILER_IS_GNUCXX)
    SET (CMAKE_CXX_FLAGS "-std=c++17 ${CXX_FLAGS_Common} -O3  -lpthread -march=native -mtune=native -lnuma -fopenmp") # -levent
    message(STATUS "optional:-std=c++17")
endif(CMAKE_COMPILER_IS_GNUCXX)
set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(../../lib/openssl/include)
add_subdirectory(src)


add_executable(terrace_vertex test/terrace_test_vertex.cpp)
target_link_libraries(terrace_vertex terrc)

add_executable(terrace_edge test/terrace_test_edge.cpp)
target_link_libraries(terrace_edge terrc)

add_executable(terrace_alg test/terrace_test_alg.cpp)
target_link_libraries(terrace_alg terrc)

add_executable(terrace_sssp test/terrace_test_sssp.cpp)
target_link_libraries(terrace_sssp terrc)
#  -src 9 -maxiters 5 -gname slashdot -f ../../../data/slashdot.adj



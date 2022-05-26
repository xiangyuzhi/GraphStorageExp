cmake_minimum_required(VERSION 3.0)
project(exp)



add_subdirectory(sys/terrace)
add_subdirectory(sys/aspen)
#add_subdirectory(sys/stinger)
#add_subdirectory(sys/pcsr)
#add_subdirectory(sys/risgraph)
#add_subdirectory(sys/livegraph)
#add_subdirectory(sys/graphone)
#add_subdirectory(sys/llama)
#add_subdirectory(sys/teseo)
add_subdirectory(sys/ligra)
add_subdirectory(exp)

#set(CMAKE_CXX_COMPILER /home/zxy/Desktop/Code/cilk/OpenCilk-12.0.0-Linux/bin/clang++)
#set(CMAKE_CXX_STANDARD 17)
#
#include_directories(lib/terrace/include)
#include_directories(/home/zxy/Desktop/Code/openssl-3.0.2/include)
#add_subdirectory(lib/terrace/src)
#
#add_executable(test exp/test.cpp)
#target_link_libraries(test terrace)
#  -src 9 -maxiters 5 -f ../data/slashdot.adj




set(CMAKE_CXX_STANDARD 17)
if(CMAKE_COMPILER_IS_GNUCXX)
    SET (CMAKE_CXX_FLAGS "-std=c++17 ${CXX_FLAGS_Common} -O3 -fopenmp")
    message(STATUS "optional:-std=c++17")
endif(CMAKE_COMPILER_IS_GNUCXX)

include_directories(include)
include_directories(include/utils)
include_directories(include/algorithms)

add_executable(pcsr_alg test/pcsr_test_alg.cpp)
add_executable(pcsr_edge test/pcsr_test_edge.cpp)
add_executable(pcsr_vertex test/pcsr_test_vertex.cpp)
#  -src 9 -maxiters 5 -f ../../../data/slashdot.adj

set(CMAKE_CXX_STANDARD 14)


if(CMAKE_COMPILER_IS_GNUCXX)
    SET (CMAKE_CXX_FLAGS "-std=c++14 ${CXX_FLAGS_Common} -O3 -DEDGELONG -mcx16 -march=native -fcilkplus -fopenmp -pthread")
    message(STATUS "optional:-std=c++14")
endif(CMAKE_COMPILER_IS_GNUCXX)

include_directories(include)
include_directories(../../lib)

# ./run_static_algorithm -t BFS -src 1 -r 4 -s -f ../../../data/ADJgraph/LiveJournal.adj
add_executable(aspen_tst test/aspen_test.cpp)

add_executable(aspen_alg test/aspen_test_alg.cpp)

add_executable(aspen_vertex test/aspen_test_vertex.cpp)

add_executable(aspen_edge test/aspen_test_edge.cpp)

# -s -f ../../../data/LiveJournal.adj
#add_executable(aspen test/test_mem.cpp)
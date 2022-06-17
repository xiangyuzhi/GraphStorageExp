

set(CMAKE_CXX_STANDARD 17)
if(CMAKE_COMPILER_IS_GNUCXX)
    SET (CMAKE_CXX_FLAGS "-std=c++17 ${CXX_FLAGS_Common} -O3 -DEDGELONG -mcx16 -march=native -fcilkplus -fopenmp -pthread -ldl")
    message(STATUS "optional:-std=c++17")
endif(CMAKE_COMPILER_IS_GNUCXX)

include_directories(include)
include_directories(../../lib)
include_directories(../../exp/memory_tst)
include_directories(../../exp/include)

# ./run_static_algorithm -t BFS -src 1 -r 4 -s -f ../../../data/ADJgraph/LiveJournal.adj
#add_executable(aspen_tst test/aspen_test.cpp)
#
#add_executable(aspen_alg test/aspen_test_alg.cpp)
#
#add_executable(aspen_vertex test/aspen_test_vertex.cpp)
#
#add_executable(aspen_edge test/aspen_test_edge.cpp)

#add_executable(exp_aspen test/exp_aspen.cpp)

add_executable(exp_aspen_e exp/exp_aspen_e.cpp)

add_executable(exp_aspen_alg exp/exp_aspen_alg.cpp)

# -s -f ../../../data/LiveJournal.adj
#add_executable(aspen test/test_mem.cpp)
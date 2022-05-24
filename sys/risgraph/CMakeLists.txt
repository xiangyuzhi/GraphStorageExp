cmake_minimum_required(VERSION 3.0)
project(risgraph)


if(CMAKE_COMPILER_IS_GNUCXX)
    SET (CMAKE_CXX_FLAGS "-std=c++14 ${CXX_FLAGS_Common} -O3 -DEDGELONG -mcx16 -march=native -fcilkplus -pthread")
    message(STATUS "optional:-std=c++14")
endif(CMAKE_COMPILER_IS_GNUCXX)



include_directories(include)
include_directories(include/algorithms)

include_directories(../../lib/tbb/include)
include_directories(../../lib/sparsehash/src)
include_directories(../../lib/cpp-art/include)
include_directories(../../lib/abseil-cpp)

link_directories(../../lib/tbb/)

set(LINK_FLAGS_COMMON "-fopenmp")
set(COMPILE_FLAGS64 "${COMPILE_FLAGS_COMMON} -DB64")

add_executable(risgraph test/risgraph_test_alg.cpp)
target_link_libraries(risgraph tbb)
set_target_properties(risgraph PROPERTIES  COMPILE_FLAGS ${COMPILE_FLAGS64}
        LINK_FLAGS ${LINK_FLAGS_COMMON})
#  -src 9 -maxiters 5 -f ../../../data/slashdot.adj
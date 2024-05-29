cmake_minimum_required(VERSION 3.0)
project(risgraph)


if(CMAKE_COMPILER_IS_GNUCXX)
    SET (CMAKE_CXX_FLAGS "-std=c++14 ${CXX_FLAGS_Common} -DEDGELONG -mcx16 -march=native -fcilkplus -pthread -fopenmp")
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

add_executable(risgraph_alg test/risgraph_test_alg.cpp)
target_link_libraries(risgraph_alg tbb)
set_target_properties(risgraph_alg PROPERTIES  COMPILE_FLAGS ${COMPILE_FLAGS64}
        LINK_FLAGS ${LINK_FLAGS_COMMON})

add_executable(risgraph_edge test/risgraph_test_edge.cpp)
target_link_libraries(risgraph_edge tbb)
set_target_properties(risgraph_edge PROPERTIES  COMPILE_FLAGS ${COMPILE_FLAGS64}
        LINK_FLAGS ${LINK_FLAGS_COMMON})

add_executable(risgraph_propotion test/risgraph_test_propotion.cpp)
target_link_libraries(risgraph_propotion tbb)
set_target_properties(risgraph_propotion PROPERTIES  COMPILE_FLAGS ${COMPILE_FLAGS64}
        LINK_FLAGS ${LINK_FLAGS_COMMON})

add_executable(risgraph_mem test/risgraph_test_memory.cpp)
target_link_libraries(risgraph_mem tbb)
set_target_properties(risgraph_mem PROPERTIES  COMPILE_FLAGS ${COMPILE_FLAGS64}
        LINK_FLAGS ${LINK_FLAGS_COMMON})

add_executable(risgraph_scala test/risgraph_test_scala.cpp)
target_link_libraries(risgraph_scala tbb)
set_target_properties(risgraph_scala PROPERTIES  COMPILE_FLAGS ${COMPILE_FLAGS64}
        LINK_FLAGS ${LINK_FLAGS_COMMON})


add_executable(risgraph_tst test/risgraph_test.cpp)
target_link_libraries(risgraph_tst tbb)
set_target_properties(risgraph_tst PROPERTIES  COMPILE_FLAGS ${COMPILE_FLAGS64}
        LINK_FLAGS ${LINK_FLAGS_COMMON})
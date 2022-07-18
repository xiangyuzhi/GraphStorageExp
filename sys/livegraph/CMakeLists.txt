
set(CMAKE_CXX_STANDARD 17)
if(CMAKE_COMPILER_IS_GNUCXX)
    SET (CMAKE_CXX_FLAGS "-std=c++17 ${CXX_FLAGS_Common} -O3 -march=native -mfma -fopenmp -fcilkplus")
    message(STATUS "optional:-std=c++17")
endif(CMAKE_COMPILER_IS_GNUCXX)

include_directories(../../lib/tbb/include)
include_directories(include/utils)

link_directories(../../lib/tbb/)

find_package(Threads REQUIRED)
link_libraries(Threads::Threads)

aux_source_directory(src SRC_DIR)
include_directories(include)
add_library(lg ${SRC_DIR})
include_directories(include/algorithms)

add_executable(livegraph_vertex test/livegraph_test_vertex.cpp)
target_link_libraries(livegraph_vertex lg)
target_link_libraries(livegraph_vertex tbb)

add_executable(livegraph_edge test/livegraph_test_edge.cpp)
target_link_libraries(livegraph_edge lg)
target_link_libraries(livegraph_edge tbb)

add_executable(livegraph_alg test/livegraph_test_alg.cpp)
target_link_libraries(livegraph_alg lg)
target_link_libraries(livegraph_alg tbb)

#  -src 9 -maxiters 5 -f ../../../data/slashdot.adj
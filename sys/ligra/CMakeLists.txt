
set(CMAKE_CXX_STANDARD 17)
if(CMAKE_COMPILER_IS_GNUCXX)
    SET (CMAKE_CXX_FLAGS "-std=c++17 ${CXX_FLAGS_Common} -O3 -fopenmp")
    message(STATUS "optional:-std=c++17")
endif(CMAKE_COMPILER_IS_GNUCXX)

include_directories(include)
include_directories(include/utils)
include_directories(include/algorithms)

add_library(csr SHARED src/csr.cpp)

add_executable(ligra_tst test/csr_test.cpp)
target_link_libraries(ligra_tst csr)

add_executable(ligra_vertex test/csr_test_vertex.cpp)
target_link_libraries(ligra_vertex csr)

add_executable(ligra_edge test/csr_test_edge.cpp)
target_link_libraries(ligra_edge csr)

add_executable(ligra_alg test/csr_test_alg.cpp)
target_link_libraries(ligra_alg csr)
cmake_minimum_required(VERSION 3.10)
project(teseo)


set(CMAKE_CXX_COMPILER /usr/bin/g++-9)

set(CMAKE_CXX_STANDARD 17)
if(CMAKE_COMPILER_IS_GNUCXX)
    SET (CMAKE_CXX_FLAGS "-std=c++17 ${CXX_FLAGS_Common} -O3 -lpthread -levent_pthreads -march=native -mtune=native -levent -fopenmp") #
    message(STATUS "optional:-std=c++17")
endif(CMAKE_COMPILER_IS_GNUCXX)

add_subdirectory(src)

#include_directories(include/teseo)
include_directories(include)
include_directories(include/utils)
include_directories(include/algorithms)
include_directories(driver)

add_library(td SHARED driver/teseo_openmp.cpp)


add_executable(teseo_tst tests/teseo_test.cpp)
TARGET_LINK_LIBRARIES(teseo_tst teseo1 td)

add_executable(teseo_alg tests/teseo_test_alg.cpp)
TARGET_LINK_LIBRARIES(teseo_alg teseo1 td)

add_executable(teseo_edge tests/teseo_test_edge.cpp)
TARGET_LINK_LIBRARIES(teseo_edge teseo1 td)

add_executable(teseo_propotion tests/teseo_test_propotion.cpp)
TARGET_LINK_LIBRARIES(teseo_propotion teseo1 td)

add_executable(teseo_mem tests/teseo_test_memory.cpp)
TARGET_LINK_LIBRARIES(teseo_mem teseo1 td)

add_executable(teseo_scala tests/teseo_test_scala.cpp)
TARGET_LINK_LIBRARIES(teseo_scala teseo1 td)

#add_executable(teseo_vertex tests/teseo_test_vertex.cpp)
#TARGET_LINK_LIBRARIES(teseo_vertex teseo1)
#
#add_executable(teseo_tst_d tests/TST_driver.cpp)
#TARGET_LINK_LIBRARIES(teseo_tst_d teseo1 td)
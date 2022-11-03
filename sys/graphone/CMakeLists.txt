#cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
#project(graphone)

#set(CMAKE_CXX_STANDARD 17)  # enable C++11 standard
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS ON)

# set(COMPILE_FLAGS_COMMON "-std=c++1y -O3 -g -DOVER_COMMIT -DTBB -DPLAIN_GRAPH -Wno-unused-result -fopenmp")
set(COMPILE_FLAGS_COMMON "-std=gnu++1z -g -DOVER_COMMIT -Wno-unused-result -DPLAIN_GRAPH -fopenmp ")

set(COMPILE_FLAGS64 "${COMPILE_FLAGS_COMMON} -DB64 ")
set(LINK_FLAGS_COMMON "-fopenmp")

include_directories(../../lib/graph1lib)
include_directories(src)
include_directories(include/gview)
include_directories(include/onedata)
include_directories(include/analytics)
include_directories(test)
include_directories(include/utils)
include_directories(include/algorithms)

add_subdirectory(src)
add_subdirectory(include/onedata)

add_executable(graphone_alg test/graph1_test_alg.cpp)
set_target_properties(graphone_alg PROPERTIES  COMPILE_FLAGS ${COMPILE_FLAGS64}
                      LINK_FLAGS ${LINK_FLAGS_COMMON})
target_link_libraries(graphone_alg src64 onedata64 )

add_executable(graphone_edge test/graph1_test_edge.cpp)
set_target_properties(graphone_edge PROPERTIES  COMPILE_FLAGS ${COMPILE_FLAGS64}
        LINK_FLAGS ${LINK_FLAGS_COMMON})
target_link_libraries(graphone_edge src64 onedata64 )

add_executable(graphone_mem test/graph1_test_memory.cpp)
set_target_properties(graphone_mem PROPERTIES  COMPILE_FLAGS ${COMPILE_FLAGS64}
        LINK_FLAGS ${LINK_FLAGS_COMMON})
target_link_libraries(graphone_mem src64 onedata64 )


add_executable(graphone_scala test/graph1_test_scala.cpp)
set_target_properties(graphone_scala PROPERTIES  COMPILE_FLAGS ${COMPILE_FLAGS64}
        LINK_FLAGS ${LINK_FLAGS_COMMON})
target_link_libraries(graphone_scala src64 onedata64 )

#add_executable(graph1tst graph1_tst.cpp)
#set_target_properties(graph1tst PROPERTIES  COMPILE_FLAGS ${COMPILE_FLAGS64}
#        LINK_FLAGS ${LINK_FLAGS_COMMON})
#target_link_libraries(graph1tst src64 onedata64 test64)

# target_link_libraries(graphone64 src64 onedata64 test64 -ltbb)
#target_link_libraries(graphone64 src64 onedata64 test64 -ltbb -ltcmalloc_minimal)


#set(graphone_source_files graph1_tst.cpp)
#add_executable(graphone32 ${graphone_source_files})
#set_target_properties(graphone32 PROPERTIES  COMPILE_FLAGS ${COMPILE_FLAGS32}
#                      LINK_FLAGS ${LINK_FLAGS_COMMON})
#target_link_libraries(graphone32 src32 onedata32 test32)
# target_link_libraries(graphone32 src32 onedata32 test32 -ltbb)
#target_link_libraries(graphone32 src32 onedata32 test32 -ltbb -ltcmalloc_minimal)

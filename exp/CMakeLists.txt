
include_directories(data_proc/include)

set(CMAKE_CXX_STANDARD 17)
if(CMAKE_COMPILER_IS_GNUCXX)
    SET (CMAKE_CXX_FLAGS "-std=c++17 ${CXX_FLAGS_Common} -O3  -lpthread -march=native -mtune=native -fopenmp") # -levent
    message(STATUS "optional:-std=c++17")
endif(CMAKE_COMPILER_IS_GNUCXX)


add_executable(read_adj data_proc/read_adj.cpp)

add_executable(read_tsv data_proc/read_tsv.cpp)

add_executable(transfer data_proc/transfer.cpp)

add_executable(test_memory memory_tst/test_memory.cpp)
target_link_libraries(test_memory -ldl)

add_executable(read_ldbc data_proc/read_ldbc.cpp)

#add_executable(generate_rmat data_proc/generate_rmat_graph.cpp)
#aux_source_directory(teseo_graph/reader DIR_LIB_SRCS)
#aux_source_directory(teseo_graph/common DIR_LIB_SRCS)
#aux_source_directory(teseo_graph/edge DIR_LIB_SRCS)
#add_library(reader SHARED ${DIR_LIB_SRCS})
#include_directories(teseo_graph)
#add_executable(read_teseo teseo_graph/read_teseo.cpp)
#target_link_libraries(read_teseo reader -lpapi -lnuma)



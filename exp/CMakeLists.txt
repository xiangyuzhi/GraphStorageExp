
include_directories(data_proc/include)

set(CMAKE_CXX_STANDARD 17)
if(CMAKE_COMPILER_IS_GNUCXX)
    SET (CMAKE_CXX_FLAGS "-std=c++17 ${CXX_FLAGS_Common} -O3  -lpthread -march=native -mtune=native -lnuma -fopenmp") # -levent
    message(STATUS "optional:-std=c++17")
endif(CMAKE_COMPILER_IS_GNUCXX)
set(CMAKE_CXX_STANDARD 17)


add_executable(read_adj data_proc/read_adj.cpp)

add_executable(read_tsv data_proc/read_tsv.cpp)

add_executable(transfer data_proc/transfer.cpp)

add_executable(test_memory memory_tst/test_memory.cpp)
target_link_libraries(test_memory -ldl)



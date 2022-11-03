# set(LIBHELLO_SRC teseo.cpp)

# put all the cpp file into the src to compile

include_directories(../include)
aux_source_directory(. DIR_LIB_SRCS)
aux_source_directory(auxiliary DIR_LIB_SRCS)
aux_source_directory(context DIR_LIB_SRCS)
aux_source_directory(gc DIR_LIB_SRCS)
aux_source_directory(interface DIR_LIB_SRCS)
aux_source_directory(memstore DIR_LIB_SRCS)
aux_source_directory(profiler DIR_LIB_SRCS)
aux_source_directory(rebalance DIR_LIB_SRCS)
aux_source_directory(runtime DIR_LIB_SRCS)
aux_source_directory(third-party DIR_LIB_SRCS)
aux_source_directory(transaction DIR_LIB_SRCS)
aux_source_directory(util DIR_LIB_SRCS)

message(${DIR_LIB_SRCS})
# message(${DIR_LIB_SRCS2})

add_library(teseo1 SHARED ${DIR_LIB_SRCS})
TARGET_LINK_LIBRARIES(teseo1 event)
TARGET_LINK_LIBRARIES(teseo1 event_pthreads)
#ADD_LIBRARY(teseo_static STATIC ${LIBHELLO_SRC})

#SET_TARGET_PROPERTIES(teseo_static PROPERTIES OUTPUT_NAME "teseo")

SET_TARGET_PROPERTIES(teseo1 PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(teseo_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

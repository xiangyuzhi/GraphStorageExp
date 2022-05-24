aux_source_directory(stinger_alg DIR_LIB_SRCS)
aux_source_directory(stinger_core DIR_LIB_SRCS)
aux_source_directory(stinger_utils DIR_LIB_SRCS)

include_directories(../include)
include_directories(../include/stinger_utils)
include_directories(../include/stinger_core)
include_directories(../include/stinger_alg)


add_library(stinger ${DIR_LIB_SRCS} )
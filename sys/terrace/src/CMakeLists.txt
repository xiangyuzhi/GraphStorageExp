
aux_source_directory(. DIR_LIB_SRCS)
aux_source_directory(gqf DIR_LIB_SRCS)

#SET(LIBRARY_OUTPUT_PATH ../lib)

add_library(terrc SHARED ${DIR_LIB_SRCS})
# INSTALL(TARGETS terrace LIBRARY DESTINATION lib)

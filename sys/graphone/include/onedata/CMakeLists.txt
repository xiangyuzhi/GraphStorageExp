set(onedata_source_files
    graph_base.h
    onenum.h
    oneblob.h
    onestr.h
    onestr.cpp
    onegraph.h
    onekv.h
    vunit.h
    mem_pool.h
)


add_library(onedata64 ${onedata_source_files})
set_target_properties(onedata64 PROPERTIES  COMPILE_FLAGS ${COMPILE_FLAGS64}
                      LINK_FLAGS ${LINK_FLAGS_COMMON})

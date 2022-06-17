#ifndef EXP_ASPEN_TEST_H
#define EXP_ASPEN_TEST_H

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cmath>
#include <sys/mman.h>
#include <stdio.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include "sys/time.h"

#include "pbbslib/strings/string_basics.h"
#include "pbbslib/sequence_ops.h"
#include "pbbslib/monoid.h"

#include "graph/versioned_graph.h"
#include "common/compression.h"
#include "graph/api.h"

typedef unsigned long uintE;
typedef unsigned int uintV;
using treeplus_graph = traversable_graph<sym_immutable_graph_tree_plus>;


#define PRINT(x) do { \
	{ std::cout << x << std::endl; } \
} while (0)

uint32_t num_nodes;
uint64_t num_edges;
std::string src, dest;
struct timeval t_start, t_end;
struct timezone tzp;

versioned_graph<treeplus_graph>*G ;

auto load_graph(commandLine& P){
    return initialize_treeplus_graph(P);
}

auto init_graph(uint32_t VN){
    size_t n = VN;
    size_t m = 0;

    uintE* offsets = pbbs::new_array_no_init<uintE>(n);
    uintV* edges = pbbs::new_array_no_init<uintV>(m);

    std::tie(n, m, offsets, edges) =  make_tuple(n, m, offsets, edges);
    return versioned_graph<treeplus_graph>(n, m, offsets, edges);
}

#endif //EXP_ASPEN_TEST_H

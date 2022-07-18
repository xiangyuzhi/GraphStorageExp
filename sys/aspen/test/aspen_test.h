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
//#include "utils/io_util.h"
//#include "utils/util.h"
//#include "utils/rmat_util.h"

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

double cal_time(std::vector<double> timelist){
    if(timelist.size() == 1) return timelist[0];
    sort(timelist.begin(),timelist.end());
    double st = 0.0;
    for(uint32_t i = 1 ;i < timelist.size()-1;i++)
        st += timelist[i];
    return st/double(timelist.size()-2);
}

static std::string getCurrentTime0() {
    std::time_t result = std::time(nullptr);
    std::string ret;
    ret.resize(64);
    int wsize = sprintf((char *)&ret[0], "%s", std::ctime(&result));
    ret.resize(wsize-1);
    return ret;
}

auto init_graph(uint32_t VN){
    size_t n = VN;
    size_t m = 0;

    uintE* offsets = pbbs::new_array_no_init<uintE>(n);
    uintV* edges = pbbs::new_array_no_init<uintV>(m);

    std::tie(n, m, offsets, edges) =  make_tuple(n, m, offsets, edges);
    return versioned_graph<treeplus_graph>(n, m, offsets, edges);
}

void print_time_elapsed(std::string desc, struct timeval* start, struct
        timeval* end)
{
    struct timeval elapsed;
    if (start->tv_usec > end->tv_usec) {
        end->tv_usec += 1000000;
        end->tv_sec--;
    }
    elapsed.tv_usec = end->tv_usec - start->tv_usec;
    elapsed.tv_sec = end->tv_sec - start->tv_sec;
    float time_elapsed = (elapsed.tv_sec * 1000000 + elapsed.tv_usec)/1000000.f;
    std::cout << desc << "Total Time Elapsed: " << std::to_string(time_elapsed) <<
              "seconds" << std::endl;
}

float cal_time_elapsed(struct timeval* start, struct timeval* end)
{
    struct timeval elapsed;
    if (start->tv_usec > end->tv_usec) {
        end->tv_usec += 1000000;
        end->tv_sec--;
    }
    elapsed.tv_usec = end->tv_usec - start->tv_usec;
    elapsed.tv_sec = end->tv_sec - start->tv_sec;
    return (elapsed.tv_sec * 1000000 + elapsed.tv_usec)/1000000.f;
}

std::vector<uint32_t> get_random_permutation(uint32_t num) {
    std::vector<uint32_t> perm(num);
    std::vector<uint32_t> vec(num);

    for (uint32_t i = 0; i < num; i++)
        vec[i] = i;

    uint32_t cnt{0};
    while (vec.size()) {
        uint32_t n = vec.size();
        srand(time(NULL));
        uint32_t idx = rand() % n;
        uint32_t val = vec[idx];
        std::swap(vec[idx], vec[n-1]);
        vec.pop_back();
        perm[cnt++] = val;
    }
    return perm;
}

#endif //EXP_ASPEN_TEST_H

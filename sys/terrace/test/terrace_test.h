//
// Created by zxy on 4/26/22.
//

#ifndef EXP_TEST_H
#define EXP_TEST_H


#include <stdlib.h>
#include <assert.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <random>
#include <algorithm>
#include <queue>
#include "sys/time.h"

#include "graph.h"
#include "parallel.h"
#include "util.h"
#include "parse_command_line.h"
#include "rmat_util.h"

#include "gabps/bitmap.h"
#include "gabps/platform_atomics.h"
#include "gabps/pvector.h"
// #include "gabps/sliding_queue.h"

#include "integerSort/blockRadixSort/blockRadixSort.h"
#include "PMA.hpp"
#include "io_util.h"
using namespace graphstore;


Graph *G;
std::vector<uint32_t> new_srcs;
std::vector<uint32_t> new_dests;
std::vector<uint32_t> query_srcs;
std::vector<uint32_t> query_dests;
uint32_t num_nodes;
uint64_t num_edges;
std::string src, dest;
struct timeval t_start, t_end;
struct timezone tzp;


void del_graph(){
    if (G!=NULL)
        free(G);
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




#endif //EXP_TEST_H

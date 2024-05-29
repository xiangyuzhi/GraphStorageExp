//
// Created by zxy on 5/7/22.
//

#ifndef EXP_LIVEGRAPH_TEST_H
#define EXP_LIVEGRAPH_TEST_H

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
#include "io_util.h"
#include "util.h"
#include "rmat_util.h"
#include "livegraph.hpp"
#include "tbb/concurrent_hash_map.h"
#include "parallel.h"
#include "omp.h"
namespace impl = livegraph;
using namespace std;

#define PRINT(x) do { \
	{ std::cout << x << std::endl; } \
} while (0)

using vertex_dictionary_t = tbb::concurrent_hash_map<uint64_t, lg::vertex_t>;
#define VertexDictionary reinterpret_cast<vertex_dictionary_t*>(m_pHashMap)
void* m_pHashMap;

lg::vertex_t internal_id = 0;
lg::Graph* G;
std::vector<uint32_t> new_srcs;
std::vector<uint32_t> new_dests;
std::vector<uint32_t> query_srcs;
std::vector<uint32_t> query_dests;
uint32_t num_nodes;
uint64_t num_edges;
std::string src, dest;
struct timeval t_start, t_end;
struct timezone tzp;

void del_G(){
    delete G;
    G = nullptr;
    delete VertexDictionary;
    m_pHashMap = nullptr;

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
    time_t result = time(nullptr);
    std::string ret;
    ret.resize(64);
    int wsize = sprintf((char *)&ret[0], "%s", ctime(&result));
    ret.resize(wsize-1);
    return ret;
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





struct commandLine {
    int argc;
    char** argv;
    std::string comLine;
    commandLine(int _c, char** _v, std::string _cl)
            : argc(_c), argv(_v), comLine(_cl) {
        if (getOption("-h") || getOption("-help"))
            badArgument();
    }

    commandLine(int _c, char** _v)
            : argc(_c), argv(_v), comLine("bad arguments") { }

    void badArgument() {
        std::cout << "usage: " << argv[0] << " " << comLine << std::endl;
        exit(0);
    }

    // get an argument
    // i is indexed from the last argument = 0, second to last indexed 1, ..
    char* getArgument(int i) {
        if (argc < 2+i) badArgument();
        return argv[argc-1-i];
    }

    // looks for two filenames
    std::pair<char*,char*> IOFileNames() {
        if (argc < 3) badArgument();
        return std::pair<char*,char*>(argv[argc-2],argv[argc-1]);
    }

    std::pair<size_t,char*> sizeAndFileName() {
        if (argc < 3) badArgument();
        return std::pair<size_t,char*>(std::atoi(argv[argc-2]),(char*) argv[argc-1]);
    }

    bool getOption(std::string option) {
        for (int i = 1; i < argc; i++)
            if ((std::string) argv[i] == option) return true;
        return false;
    }

    char* getOptionValue(std::string option) {
        for (int i = 1; i < argc-1; i++)
            if ((std::string) argv[i] == option) return argv[i+1];
        return NULL;
    }

    std::string getOptionValue(std::string option, std::string defaultValue) {
        for (int i = 1; i < argc-1; i++)
            if ((std::string) argv[i] == option) return (std::string) argv[i+1];
        return defaultValue;
    }

    long getOptionLongValue(std::string option, long defaultValue) {
        for (int i = 1; i < argc-1; i++)
            if ((std::string) argv[i] == option) {
                long r = atol(argv[i+1]);
                if (r < 0) badArgument();
                return r;
            }
        return defaultValue;
    }

    int getOptionIntValue(std::string option, int defaultValue) {
        for (int i = 1; i < argc-1; i++)
            if ((std::string) argv[i] == option) {
                int r = atoi(argv[i+1]);
                if (r < 0) badArgument();
                return r;
            }
        return defaultValue;
    }

    double getOptionDoubleValue(std::string option, double defaultValue) {
        for (int i = 1; i < argc-1; i++)
            if ((std::string) argv[i] == option) {
                double val;
                if (sscanf(argv[i+1], "%lf",  &val) == EOF) {
                    badArgument();
                }
                return val;
            }
        return defaultValue;
    }

};


void load_graph(commandLine& P){
    auto filename = P.getOptionValue("-f", "none");
    pair_uint *edges = get_edges_from_file_adj_sym(filename.c_str(), &num_edges, &num_nodes);
    G = new lg::Graph();
    m_pHashMap = new tbb::concurrent_hash_map<uint64_t, /* vertex_t */ uint64_t>();
    auto tx = G->begin_transaction();
    for(uint64_t id=0; id<num_nodes; id++){
        vertex_dictionary_t::accessor accessor; // xlock
        VertexDictionary->insert(accessor, id);
        internal_id = tx.new_vertex();
        string_view data {};
        tx.put_vertex(internal_id, data);
        accessor->second = internal_id;
    }
    tx.commit();

    for (uint32_t i = 0; i < num_edges; i++) {
        new_srcs.push_back(edges[i].x);
        new_dests.push_back(edges[i].y);
    }
    auto perm = get_random_permutation(num_edges);

    PRINT("=============== Load Graph BEGIN ===============");
    gettimeofday(&t_start, &tzp);
    auto tx2 = G->begin_transaction();
    vertex_dictionary_t::const_accessor accessor1, accessor2;  // shared lock on the dictionary
    for (uint32_t i =0 ; i< num_edges;i++){
        VertexDictionary->find(accessor1, new_srcs[perm[i]]);
        VertexDictionary->find(accessor2, new_dests[perm[i]]);
        lg::vertex_t internal_source_id = accessor1->second;
        lg::vertex_t internal_destination_id = accessor2->second;
        int w = 1;string_view weight { (char*) &w, sizeof(w) };
        tx2.put_edge(internal_source_id, /* label */ 0, internal_destination_id, weight);
    }
    tx2.commit();
    gettimeofday(&t_end, &tzp);
    free(edges);
    new_srcs.clear();
    new_dests.clear();
//    float size_gb = G->get_size() / (float) 1073741824;
//    PRINT("Load Graph: Nodes: " << G->get_n() <<" Edges: " << G->edges.N<< " Size: " << size_gb << " GB");
    PRINT(">>> Load Graph: Nodes: " << G->get_max_vertex_id() <<" Edges: " << num_edges );
    print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);
    PRINT("Throughput: " << (G->get_max_vertex_id()+num_edges) / (float) cal_time_elapsed(&t_start, &t_end));
    PRINT("================ Load Graph END ================");
}



#endif //EXP_LIVEGRAPH_TEST_H

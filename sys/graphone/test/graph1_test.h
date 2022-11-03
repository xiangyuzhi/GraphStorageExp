//
// Created by zxy on 5/6/22.
//

#ifndef EXP_GRAPH1_TEST_H
#define EXP_GRAPH1_TEST_H

#include <stdlib.h>
#include <assert.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <random>
#include <algorithm>
#include <string>
#include <queue>
#include "sys/time.h"
#include "util/io_util.h"
#include "util/util.h"
#include "util/rmat_util.h"
#include "graph.h"

#include "typekv.h"
#include "graph.h" // main graph instance
#include "sgraph.h" // directed and undirected weighted graphs
#include "str.h" // dictionary external vertex id to logical id
#include "type.h" // graphone typedefs
#include "typekv.h" // first container, reserved for the "metadata"
#include "graph_view.h" // static view
#include "util/spinlock.hpp"
#include "util/parse_command_line.h"


#define PRINT(x) do { \
	{ std::cout << x << std::endl; } \
} while (0)

graph* g;
std::vector<uint32_t> new_srcs;
std::vector<uint32_t> new_dests;
uint32_t num_nodes;
uint64_t num_edges;
std::string src, dest;
struct timeval t_start, t_end;
struct timezone tzp;

void del_G(){
    if (g!=NULL)
        free(g);
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


struct PaddedLock { // to avoid false sharing
    common::SpinLock m_lock;
    uint64_t padding[7];
};
PaddedLock* m_edge_locks { nullptr };

int THD_COUNT = 0;
uint64_t m_num_edge_locks { 0 };
common::SpinLock m_mutex_vtx;
bool m_is_directed = true;

void graph1_init(uint64_t max_num_vertices){

    THD_COUNT = (omp_get_max_threads() -1); // as defined by the experiments in the GraphOne suite
    g->cf_info = new cfinfo_t*[2]; // number of containers ("column families") = 2: 1. metadata (vertex dictionary), 2. edges of the graph
    g->p_info = nullptr; // property names, unused
    // metadata
    auto metadata = new typekv_t(); // vertex types or classes
    g->add_columnfamily(metadata); // cf_info[0] <- typekv_t
    // the actual weighted graph
    pgraph_t<lite_edge_t>* weighted_graph { nullptr };
    if(!m_is_directed){
        weighted_graph = new p_ugraph_t(); // undirected
    } else {
        weighted_graph = new p_dgraph_t(); // directed
    }
    weighted_graph->flag1 = weighted_graph->flag2 =1; // `1' is the vertex type
    g->add_columnfamily(weighted_graph); // register the graph in the database instance
    metadata->manual_setup(/* max number of vertices */ max_num_vertices, /* statically create the vertices ? */ false);
    g->prep_graph_baseline(); // I think this finalises the schema
    g->create_threads(/* archiving */ true, /* logging */ false); // background threads, to create the snapshots and logging to disk
    // edge locks, to ensure consistency when performing an update
    bool use_vertex2id_mapping = false, blind_writes = false;
    if(use_vertex2id_mapping || !blind_writes){
        m_num_edge_locks = 64; // arbitrary value
        static_assert(sizeof(PaddedLock) == 64, "Expected to match the size of a cache line");
        m_edge_locks = new PaddedLock[ m_num_edge_locks ]();
    }
}

void del_g(){
    delete[] m_edge_locks; m_edge_locks = nullptr; m_num_edge_locks = 0;
    delete g; g = nullptr;
}

uint64_t num_vertices() {
    return g->get_typekv()->get_type_vcount(0);
}

void graph1_add_v(uint64_t vid){
    string str_vertex_id = to_string(vid);
    // The internal dictionary provided by GraphOne is not thread safe
    scoped_lock<common::SpinLock> lock(m_mutex_vtx);
    g->type_update(str_vertex_id); // now this is always successful?
}

inline static pgraph_t<lite_edge_t>* get_graphone_graph(){
    assert(g->cf_count == 2 && "Graph not initialised");
    return (pgraph_t<lite_edge_t>*) g->get_sgraph(1);
}

std::atomic<uint64_t> m_num_levels { 0 };
std::atomic<uint64_t> m_num_edges { 0 };
void do_update(bool is_insert, uint64_t v0, uint64_t v1, double weight){

    weight_edge_t edge;
    if(is_insert){
        set_src(edge, v0); // edge source
    } else {
        set_src(edge, DEL_SID(v0)); // edge source + mask to ask for deletion
    }
    set_dst(edge, v1); // destination
    edge.dst_id.second.value_double = weight;
    edge.dst_id.second.value32b = (uint32_t)weight;
    // Perform the update
    status_t rc = get_graphone_graph()->batch_edge(edge);
    if(rc == eEndBatch) m_num_levels++;

    // update the global counter on the number of edges present
    if(is_insert){
        m_num_edges++; // atomic
    } else {
        assert(m_num_edges > 0);
        m_num_edges--; // atomic
    }
}

void graph1_add_e(uint64_t src, uint64_t dst){
    common::SpinLock& mutex = m_edge_locks[(src + dst) % m_num_edge_locks].m_lock;
    scoped_lock<common::SpinLock> xlock(mutex);
    do_update(/* is insert ? */ true, src, dst, 1.0);
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


void load_graph(commandLine& P){
    auto filename = P.getOptionValue("-f", "none");
    pair_uint *edges = get_edges_from_file_adj_sym(filename.c_str(), &num_edges, &num_nodes);
    g = new graph();
    graph1_init(num_nodes);
    for(uint64_t i=0;i<num_nodes;i++){
        graph1_add_v(i);
    }

    for (uint32_t i = 0; i < num_edges; i++) {
        new_srcs.push_back(edges[i].x);
        new_dests.push_back(edges[i].y);
    }
    auto perm = get_random_permutation(num_edges);

    PRINT("=============== Load Graph BEGIN ===============");
    gettimeofday(&t_start, &tzp);
    for (uint32_t i =0 ; i< num_edges;i++){
        graph1_add_e(new_srcs[i],new_dests[i]);
    }
    gettimeofday(&t_end, &tzp);
    free(edges);
    new_srcs.clear();
    new_dests.clear();
    float size_gb = num_vertices() / (float) 1073741824;
    PRINT("Load Graph: Nodes: " << num_vertices() <<" Edges: " << m_num_edges << " Size: " << size_gb << " GB");
    print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);
    PRINT("Throughput: " <<  num_vertices() / (float) cal_time_elapsed(&t_start, &t_end));
    PRINT("================ Load Graph END ================");
}


struct update {
    int64_t type;
    int64_t source;
    int64_t destination;
    int64_t weight;
    int64_t time;
    int64_t result;
    static int64_t get_type(const update &u) { return u.type; }
    static void set_type(update &u, int64_t v) { u.type = v; }
    static int64_t get_source(const update &u) { return u.source; }
    static void set_source(update &u, int64_t v) { u.source = v; }
    static int64_t get_dest(const update &u) { return u.destination; }
    static void set_dest(update &u, int64_t v) { u.destination = v; }
    static int64_t get_weight(const update &u) { return u.weight; }
    static int64_t get_time(const update &u) { return u.time; }
    static int64_t get_result(const update& u) { return u.result; }
    static void set_result(update &u, int64_t v) { u.result = v; }
};

typedef std::vector<update>::iterator update_iterator;

#endif //EXP_GRAPH1_TEST_H

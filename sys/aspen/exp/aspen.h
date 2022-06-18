//
// Created by 15743 on 2022/6/9.
//

#ifndef EXP_ASPEN_H
#define EXP_ASPEN_H
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
#include "trees/utils.h"

struct Edge_iter { //for edgeMap
public:
    uint64_t dst;
    uint32_t wgh;

    template<class Graph>
    Edge_iter(Graph &G, uint64_t vid){
        deg = G.find_vertex(vid).value.degree();
        ngh = G.find_vertex(vid).value.get_edges(vid);
        itr = 0;
        dst = ngh[itr];
        wgh = 1;
    }

    void next(){
        itr ++;
        dst = ngh[itr];
    }

    bool end(){
        return (itr >= deg);
    }

private:
    uint32_t deg;
    uintV *ngh;
    uint32_t itr;
};

struct aspen{
    typedef unsigned long uintE;
    typedef unsigned int uintV;
    using treeplus_graph = traversable_graph<sym_immutable_graph_tree_plus>;

    versioned_graph<treeplus_graph> *G ;
    versioned_graph<treeplus_graph>::version *S;

    void begin_read_graph(){
        S = new versioned_graph<treeplus_graph>::version(G->acquire_version());
    }

    void end_read_graph(){
        free(S); S = nullptr;
    }

    uint32_t num_vertices(){
        const auto& GA = S->graph;
        return GA.num_vertices();
    }

    void traverse_vertex(uint32_t vsize){
        begin_read_graph();
        auto& GA = S->graph;
        for (uint32_t i = 0; i < vsize; i++)
            auto a = GA.find_vertex(i);
        end_read_graph();
    }

    void load_graph(commandLine& P){
        G = new versioned_graph(initialize_treeplus_graph(P));
    }

    void init_graph(uint32_t VN){
        size_t n = VN;
        size_t m = 0;
        uintE* offsets = pbbs::new_array_no_init<uintE>(n);
        uintV* edges = pbbs::new_array_no_init<uintV>(m);
        std::tie(n, m, offsets, edges) =  make_tuple(n, m, offsets, edges);
        G = new versioned_graph<treeplus_graph>(n, m, offsets, edges);
    }

    void del(){
        free(G);G = nullptr;
    }

    void add_edges(uint32_t updates_to_run,std::pair<uint64_t, uint64_t> *raw_edges, uint nn){
        G->insert_edges_batch(updates_to_run, reinterpret_cast<tuple<uintV, uintV> *>(raw_edges), false, true, nn, false);
    }

    template <class Graph>
    bool find_e(Graph& G, uintV src, uintV dst) {
        const auto& v = G.find_vertex(src).value;
        bool found = false;
        auto map_f = [&] (uintV ngh_id, size_t ind) {
            if (dst == ngh_id) {
                found = true;
            }
        };
        v.map_elms(src, map_f);
        return found;
    }

    void read_edges(uint32_t updates_to_run,std::pair<uint64_t, uint64_t> *raw_edges){
        auto S = G->acquire_version();
        for (uint32_t i =0 ; i< updates_to_run;i++){
            find_e(S.graph,get<0>(raw_edges[i]),get<1>(raw_edges[i]));
        }
        G->release_version(std::move(S));
    }

    void del_edges(uint32_t updates_to_run,std::pair<uint64_t, uint64_t> *raw_edges, uint nn){
        G->delete_edges_batch(updates_to_run, reinterpret_cast<tuple<uintV, uintV> *>(raw_edges), false, true, nn, false);
    }

    Edge_iter get_edge_iter(uint64_t vid){
        auto& GA = S->graph;
        return Edge_iter(GA, vid);
    }

    size_t get_deg(uint64_t vid){
        auto& GA = S->graph;
        return GA.find_vertex(vid).value.degree();
    }

    uint64_t countCommon(uint32_t a, uint32_t b) {
        auto it_A = get_edge_iter(a);
        auto it_B = get_edge_iter(b);
        uint64_t ans=0;
        while (!it_A.end() && !it_B.end() && it_A.dst < a && it_B.dst < b) { //count "directed" triangles
            if (it_A.dst == it_B.dst) it_A.next(), it_B.next(), ans++;
            else if (it_A.dst < it_B.dst) it_A.next();
            else it_B.next();
        }
        return ans;
    }

};






#endif //EXP_ASPEN_H

//
// Created by zxy on 4/26/22.
//

#ifndef EXP_K_HOP_H
#define EXP_K_HOP_H

#define newA(__E,__n) (__E*) malloc((__n)*sizeof(__E))
#include "lib_extensions/sparse_table.h"
#include "lib_extensions/sequentialHT.h"
#include <unordered_set>

struct HOP_F {
    HOP_F(){}
    inline bool update (uint32_t s, uint32_t d) { //Update ShortestPathLen if found a shorter path
        return 0;
    }
    inline bool updateAtomic (uint32_t s, uint32_t d){ //atomic Update=
        return 0;
    }
    inline bool cond (uint32_t d) { return 1; }
};

//reset visited vertices
struct HOP_Vertex_F {
    HOP_Vertex_F() {}
    inline bool operator() (uint32_t i){
        return 1;
    }
};

template <typename Graph>
void K_HOP(Graph &G, uint32_t k) {
    uint64_t n = G.num_vertices();
    auto r = pbbs::random();
    timer sparse_t, dense_t, fetch_t, other_t;
    int nsrc = n/20;
    srand(n);
    for(int i = 0; i<nsrc;i++){
        uintV src = rand()%n;//r.ith_rand(n) % n;
        uint32_t tk = k;
        auto frontier = vertex_subset(n, src);
        while(tk--){
            auto output = G.edge_map(frontier, HOP_F(), sparse_t, dense_t, fetch_t, other_t, stay_dense);
            frontier.del();
            frontier = output;
        }
        frontier.del();
    }
}


template <class Graph>
void oneHop_par(Graph& G) {
    uint32_t n = G.num_vertices();
//    uint32_t i = 0;
//    while (i < n) {
//        size_t end = min(i + 1024, n);
//        uint32_t start = 0;
        parallel_for(1, n, [&](uint32_t khop_src) {
            const auto &v = G.find_vertex(khop_src).value;
            auto map_f = [&](uintV ngh_id, size_t ind) {};
            v.map_elms(khop_src, map_f);
        }, 1);
//        i += 1024;
//    }
}

template <class Graph>
void twoHop_par(Graph& G) {
    uint32_t n = G.num_vertices();
//    uint32_t i = 0;
//    while (i < n) {
//        size_t end = min(i + 1024, n);
//        uint32_t start = 0;
        parallel_for(1, n, [&](uint32_t khop_src) {
            const auto &v = G.find_vertex(khop_src).value;
            auto snd_map_f = [&] (uintV ngh, size_t ind) { };
            auto map_f = [&](uintV ngh_id, size_t ind) {
                const auto& ngh = G.find_vertex(ngh_id).value;
                ngh.map_elms(ngh_id, snd_map_f);
            };
            v.map_elms(khop_src, map_f);
        }, 1);
//        i += 1024;
//    }
}



#endif //EXP_K_HOP_H

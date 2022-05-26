//
// Created by zxy on 4/26/22.
//

#ifndef EXP_K_HOP_H
#define EXP_K_HOP_H

#define newA(__E,__n) (__E*) malloc((__n)*sizeof(__E))
#include "Map.cpp"
#include "parallel_util.h"

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

    uint64_t n = G.get_num_vertices();
    int nsrc = n/10;
    for(int i = 1; i<nsrc;i+=10){
        uint64_t src = i;//r.ith_rand(n) % n;
        uint32_t tk = k;
        VertexSubset frontier = VertexSubset(src, n);
        while(tk--){
            VertexSubset next_frontier = edgeMap(G, frontier, HOP_F(), true, 1);
            frontier.del();
            frontier = next_frontier;
        }
        frontier.del();
    }

}


//void K_HOP(Graph &G, long k) {
//    long n = G.get_num_vertices();
//    // printf("HOP_1 num_vertices %lu\n", n);
//    parallel_for (long khop_src = 0; khop_src < n ;khop_src ++){
//        VertexSubset frontier = VertexSubset(khop_src, n);
//        int tk = k;
//        while(tk--){
//            VertexSubset next_frontier = edgeMap(G, frontier, HOP_F(), true, 1);
//            frontier.del();
//            frontier = next_frontier;
//        }
//        frontier.del();
//    }
//}

#endif //EXP_K_HOP_H

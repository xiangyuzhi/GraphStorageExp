//
// Created by zxy on 5/9/22.
//

#ifndef EXP_K_HOP_H
#define EXP_K_HOP_H

#include "G-map.h"
#include "parallel_util.h"
#define newA(__E,__n) (__E*) malloc((__n)*sizeof(__E))


struct HOP_F {
    HOP_F(){}
    inline bool update (uint32_t s, uint32_t d, uint32_t w) { //Update ShortestPathLen if found a shorter path
        return 0;
    }
    inline bool updateAtomic (uint32_t s, uint32_t d, uint32_t w){ //atomic Update=
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

template <class Graph>
void K_HOP(Graph *G, long k) {

    auto tx = G->begin_read_only_transaction();
    uint64_t n = G->get_max_vertex_id();
    int nsrc = n/20;
    srand(n);
    for(int i = 0; i<nsrc;i++){
        uint32_t src = rand()%n;
        uint32_t tk = k;
        VertexSubset frontier = VertexSubset(src, n);
        while(tk--){
            VertexSubset next_frontier = edgeMap(n, tx, frontier, HOP_F(),false, true, 1);
            frontier.del();
            frontier = next_frontier;
        }
        frontier.del();
    }
    tx.abort();
}


#endif //EXP_K_HOP_H

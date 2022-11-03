//
// Created by zxy on 5/9/22.
//

#ifndef EXP_BFS_H
#define EXP_BFS_H

#include "G-map.h"

struct BFS_F {
    int32_t* Parents;
    BFS_F(int32_t* _Parents) : Parents(_Parents) {}
    inline bool update (uint32_t s, uint32_t d, int32_t w) { //Update
        if(Parents[d] == -1) { Parents[d] = s; return 1; }
        else return 0;
    }
    inline bool updateAtomic (uint32_t s, uint32_t d, int32_t w){ //atomic version of Update
        return __sync_bool_compare_and_swap(&Parents[d],-1,s);
    }
    //cond function checks if vertex has been visited yet
    inline bool cond (uint32_t d) { return (Parents[d] == -1); }
};

void BFS(OpenMP &openmp, uint32_t bfs_src) {
    uint64_t n = openmp.transaction().num_vertices();

    auto* Parents = (int32_t *) malloc(n * sizeof(uint32_t));
    parallel_for(long i = 0; i < n; i++) Parents[i] = -1;
    Parents[bfs_src] = bfs_src;

    VertexSubset frontier = VertexSubset(bfs_src, n); //creates initial frontier
    while(frontier.not_empty()){ //loop until frontier is empty
        VertexSubset next_frontier = edgeMap(openmp, frontier, BFS_F(Parents), false, true);
        frontier.del();
        frontier = next_frontier;
    }
    frontier.del();
    free(Parents);
}



#endif //EXP_BFS_H

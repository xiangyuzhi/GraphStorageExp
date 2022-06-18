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

template<typename Graph>
void BFS(Graph &G, uint32_t bfs_src) {

    uint64_t n = G.num_vertices();
    auto* Parents = (int32_t *) malloc(n * sizeof(uint32_t));
    cilk_for(long i = 0; i < n; i++) Parents[i] = -1;
    Parents[bfs_src] = bfs_src;

    VertexSubset frontier = VertexSubset(bfs_src, n); //creates initial frontier
    while(frontier.not_empty()){ //loop until frontier is empty
        VertexSubset next_frontier = edgeMap(G, frontier, BFS_F(Parents), false, true);
        frontier.del();
        frontier = next_frontier;
    }
    frontier.del();

//    std::vector<uint32_t> depths(n, UINT32_MAX);
//    for (uint32_t j = 0; j < n; j++) {
//        uint32_t current_depth = 0;
//        int32_t current_parent = j;
//        if (Parents[j] < 0) {
//            continue;
//        }
//        while (current_parent != Parents[current_parent]) {
//            current_depth += 1;
//            current_parent = Parents[current_parent];
//        }
//        depths[j] = current_depth;
//    }
//    ofstream myfile;
//    string path = "../../../log/aspen/bfs.out";
//    myfile.open (path);
//    for (int i = 0; i < n; i++) {
//        myfile << depths[i] << "\n";
//    }
//    myfile.close();

    free(Parents);

}




#endif //EXP_BFS_H

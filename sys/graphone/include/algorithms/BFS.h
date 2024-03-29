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
void BFS(Graph *G, uint32_t src) {
    long start = src;
    long n = G->v_count;
    //creates Parents array, initialized to all -1, except for start
    int32_t* Parents = (int32_t *) malloc(n * sizeof(uint32_t));
    parallel_for(long i=0;i<n;i++) Parents[i] = -1;
    Parents[start] = start;
    VertexSubset frontier = VertexSubset(start, n); //creates initial frontier
    while(frontier.not_empty()){ //loop until frontier is empty
        // printf("frontier size  %lu\n", frontier.get_n());
        // dense only
        VertexSubset next_frontier = edgeMap(G, frontier, BFS_F(Parents),false, true);
        frontier.del();
        frontier = next_frontier;
    }
    frontier.del();
    free(Parents);

#if VERIFY
    std::vector<uint32_t> depths(n, UINT32_MAX);
  for (uint32_t j = 0; j < n; j++) {
    uint32_t current_depth = 0;
    int32_t current_parent = j;
    if (Parents[j] < 0) {
      continue;
    }
    while (current_parent != Parents[current_parent]) {
      current_depth += 1;
      current_parent = Parents[current_parent];
    }
    depths[j] = current_depth;
  }

  // write out to file
  std::ofstream myfile;
  myfile.open ("bfs.out");
  for (int i = 0; i < n; i++) {
    myfile << depths[i] << "\n";
  }
  myfile.close();
#endif
}




#endif //EXP_BFS_H

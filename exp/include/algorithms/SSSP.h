//
// Created by zxy on 5/9/22.
//


#define WEIGHTED 1
#include "G-map.h"
#include "utils/parallel_util.h"


struct BF_F {
    int32_t* ShortestPathLen;
    int* Visited;
    BF_F(int32_t* _ShortestPathLen, int* _Visited) :
            ShortestPathLen(_ShortestPathLen), Visited(_Visited) {}
    inline bool update (uint32_t s, uint32_t d, int32_t edgeLen) { //Update ShortestPathLen if found a shorter path
        assert(edgeLen > 0);
        int32_t newDist = ShortestPathLen[s] + edgeLen;
        if(ShortestPathLen[d] > newDist) {
            ShortestPathLen[d] = newDist;
            if(Visited[d] == 0) { Visited[d] = 1 ; return 1;}
        }
        return 0;
    }
    inline bool updateAtomic (uint32_t s, uint32_t d, int32_t edgeLen){ //atomic Update
        int32_t newDist = ShortestPathLen[s] + edgeLen;
        return (writeMin(&ShortestPathLen[d],newDist) &&
                CAS(&Visited[d],0,1));
    }
    inline bool cond (uint32_t d) { return 1; }
};

//reset visited vertices
struct BF_Vertex_F {
    int* Visited;
    BF_Vertex_F(int* _Visited) : Visited(_Visited) {}
    inline bool operator() (uint32_t i){
        // printf("reset visited %u\n", i);
        Visited[i] = 0;
        return 1;
    }
};

template <class Graph>
void SSSP(Graph &G, long start) {
    uint64_t n = G.num_vertices();

    auto* ShortestPathLen = newA(int32_t,n);
    parallel_for(long i=0;i<n;i++) {ShortestPathLen[i] = INT_MAX/2;} //initialize ShortestPathLen to "infinity"
    ShortestPathLen[start] = 0;

    int* Visited = newA(int,n);
    parallel_for(long i=0;i<n;i++) {Visited[i] = 0;}

    VertexSubset frontier = VertexSubset(start, n);

    long round = 0;
    while(frontier.not_empty()) { // while frontier is not empty
        if (round == n) {//negative weight cycle
            parallel_for(long i=0;i<n;i++) {ShortestPathLen[i] = -(INT_MAX/2);}
            break;
        }

        VertexSubset output = edgeMap(G, frontier, BF_F(ShortestPathLen,Visited), true, true);
        vertexMap(output,BF_Vertex_F(Visited));
        frontier.del();
        frontier = output;

        round++;
    }
    frontier.del(); free(Visited);
}


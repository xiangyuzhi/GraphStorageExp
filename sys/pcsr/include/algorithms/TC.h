//
// Created by zxy on 5/10/22.
//

#ifndef EXP_TC_H
#define EXP_TC_H
#include "G-map.h"


//assumes sorted neighbor lists
template<typename Graph>
uint64_t countCommon(Graph *G, uint32_t a, uint32_t b) {

    uint64_t ans=0;
    uint32_t it_A = G->nodes[a].beginning+1, it_B = G->nodes[b].beginning+1;
    while (it_A < G->nodes[a].end && it_B < G->nodes[b].end && G->edges.items[it_A].dest < a && G->edges.items[it_B].dest < b) { //count "directed" triangles
        if (G->edges.items[it_A].dest == G->edges.items[it_B].dest) ++it_A, ++it_B, ans++;
        else if (G->edges.items[it_A].dest < G->edges.items[it_B].dest) ++it_A;
        else ++it_B;
    }

    return ans;
}

template<typename Graph>
struct countF { //for edgeMap
    Graph *G;
    std::vector<uint64_t> &counts;
    countF(Graph *G_, std::vector<uint64_t> &_counts) : G(G_), counts(_counts) {}
    inline bool update (uint32_t s, uint32_t d, uint32_t w) {
        if(s > d) {//only count "directed" triangles
            counts[8*getWorkerNum()] += countCommon(G, s, d);
        }
        return true;
    }
    inline bool updateAtomic (uint32_t s, uint32_t d, uint32_t w) {
        if (s > d) { //only count "directed" triangles
            counts[8*getWorkerNum()] += countCommon(G, s, d);
        }
        return true;
    }
    inline bool cond ([[maybe_unused]] uint32_t d) { return true; } //does nothing
};

template<typename Graph>
uint64_t TC(Graph* G) {
    uint64_t n = G->get_n();

    std::vector<uint64_t> counts(getWorkers()*8, 0);
    VertexSubset Frontier(0, n, true); //frontier contains all vertices

    edgeMap(G, Frontier, countF<Graph>(G, counts),false, false);
    Frontier.del();
    uint64_t count = 0;
    for (int i = 0; i < getWorkers(); i++) {
        count += counts[i*8];
    }
    return count;
}

#endif //EXP_TC_H

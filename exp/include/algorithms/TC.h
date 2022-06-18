//
// Created by zxy on 5/10/22.
//

#ifndef EXP_TC_H
#define EXP_TC_H
#include "G-map.h"

template<typename Graph>
struct countF { //for edgeMap
    Graph &G;
    std::vector<uint64_t> &counts;
    countF(Graph &G_, std::vector<uint64_t> &_counts) : G(G_), counts(_counts) {}
    inline bool update (uint32_t s, uint32_t d, uint32_t w) {
        if(s > d) {//only count "directed" triangles
            counts[8*getWorkerNum()] += G.countCommon(s, d);
        }
        return true;
    }
    inline bool updateAtomic (uint32_t s, uint32_t d, uint32_t w) {
        if (s > d) { //only count "directed" triangles
            counts[8*getWorkerNum()] += G.countCommon(s, d);
        }
        return true;
    }
    inline bool cond ([[maybe_unused]] uint32_t d) { return true; } //does nothing
};

template<typename Graph>
void TC(Graph& G) {
    uint64_t n = G.num_vertices();

    std::vector<uint64_t> counts(getWorkers()*8, 0);
    VertexSubset Frontier(0, n, true); //frontier contains all vertices

    edgeMap(G, Frontier, countF<Graph>(G, counts), false, false);
    Frontier.del();
    uint64_t count = 0;
    for (int i = 0; i < getWorkers(); i++) {
        count += counts[i*8];
    }
}

#endif //EXP_TC_H

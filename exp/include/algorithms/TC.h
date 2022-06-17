//
// Created by zxy on 5/10/22.
//

#ifndef EXP_TC_H
#define EXP_TC_H
#include "G-map.h"


//assumes sorted neighbor lists
template<typename Graph>
uint64_t countCommon(Graph &G, uint32_t a, uint32_t b) {

    auto edges_a = G->traverse(a);
    auto edges_b = G->traverse(b);
    auto *out_e = G->out_e();
    long ans=0;
    uint64_t it_A = edges_a.first;
    uint64_t it_B = edges_b.first;
    while (it_A<edges_a.second && it_B<edges_b.second && out_e[it_A] < a && out_e[it_B] < b) { //count "directed" triangles
        if (out_e[it_A] == out_e[it_B]) it_A++, it_B++, ans++;
        else if (out_e[it_A] < out_e[it_B]) it_A++;
        else it_B++;
    }

    return ans;
}

template<typename Graph>
struct countF { //for edgeMap
    Graph &G;
    std::vector<uint64_t> &counts;
    countF(Graph &G_, std::vector<uint64_t> &_counts) : G(G_), counts(_counts) {}
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

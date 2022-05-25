//
// Created by zxy on 5/10/22.
//

#ifndef EXP_TC_H
#define EXP_TC_H
#include "G-map.h"


//assumes sorted neighbor lists
template<typename Graph>
uint64_t countCommon(Graph *G, uint32_t a, uint32_t b) {

    auto a_ngh = G->outgoing.get_adjlist(a);
    auto b_ngh = G->outgoing.get_adjlist(b);
    uint64_t ans=0;
    uint32_t it_A = 0, it_B = 0;
    while (it_A<a_ngh.size() && it_B<b_ngh.size() && a_ngh[it_A].nbr < a && b_ngh[it_B].nbr < b) { //count "directed" triangles
        if (a_ngh[it_A].nbr == b_ngh[it_B].nbr) ++it_A, ++it_B, ans++;
        else if (a_ngh[it_A].nbr < b_ngh[it_B].nbr) ++it_A;
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
    int64_t n = G->get_vertex_num();

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

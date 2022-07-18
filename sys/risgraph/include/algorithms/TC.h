//
// Created by zxy on 5/10/22.
//

#ifndef EXP_TC_H
#define EXP_TC_H
#include "G-map.h"


//assumes sorted neighbor lists
template<typename Graph>
uint64_t countCommon(Graph *G, uint32_t a, uint32_t b) {

    auto a_edges = G->outgoing.get_adjlist(a);
    auto b_edges = G->outgoing.get_adjlist(b);
    uint32_t deg_A = a_edges.size();
    uint32_t deg_B = b_edges.size();

    uint32_t *a_ngh = newA(uint32_t, deg_A);
    uint32_t *b_ngh = newA(uint32_t, deg_B);

    for(int i=0;i<deg_A;i++) a_ngh[i] = a_edges[i].nbr;
    for(int i=0;i<deg_B;i++) b_ngh[i] = b_edges[i].nbr;


    std::sort(a_ngh,a_ngh + deg_A);
    std::sort(b_ngh,b_ngh + deg_B);

    uint64_t ans=0;
    uint32_t it_A = 0, it_B = 0;
    while (it_A!=deg_A && it_B!=deg_B && a_ngh[it_A] < a && b_ngh[it_B] < b) { //count "directed" triangles
        if (a_ngh[it_A] == b_ngh[it_B]) ++it_A, ++it_B, ans++;
        else if (a_ngh[it_A] < b_ngh[it_B]) ++it_A;
        else ++it_B;
    }
    free(a_ngh); a_ngh = nullptr;
    free(b_ngh); b_ngh = nullptr;
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

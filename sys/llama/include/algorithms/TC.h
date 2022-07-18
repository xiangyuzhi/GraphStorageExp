//
// Created by zxy on 5/10/22.
//

#ifndef EXP_TC_H
#define EXP_TC_H
#include "G-map.h"


//assumes sorted neighbor lists
template<typename Graph>
uint64_t countCommon(Graph *G, uint32_t a, uint32_t b) {

    ll_edge_iterator iter_A,iter_B;
    G->out_iter_begin(iter_A, a);
    G->out_iter_begin(iter_B, b);

    vector<uint32_t> a_ngh;
    vector<uint32_t> b_ngh;

    for(edge_t it_A = G->out_iter_next(iter_A); it_A!=LL_NIL_EDGE ;it_A = G->out_iter_next(iter_A))
        a_ngh.push_back(LL_ITER_OUT_NEXT_NODE(G, iter_A, it_A));

    for(edge_t it_B = G->out_iter_next(iter_B); it_B!=LL_NIL_EDGE ;it_B = G->out_iter_next(iter_B))
        b_ngh.push_back(LL_ITER_OUT_NEXT_NODE(G, iter_B, it_B));

    std::sort(a_ngh.begin(),a_ngh.end());
    std::sort(b_ngh.begin(),b_ngh.end());


    uint64_t ans=0;
    uint32_t it_A = 0, it_B = 0;
    while (it_A<a_ngh.size() && it_B<b_ngh.size() && a_ngh[it_A] < a && b_ngh[it_B] < b) { //count "directed" triangles
        if (a_ngh[it_A] == b_ngh[it_B]) ++it_A, ++it_B, ans++;
        else if (a_ngh[it_A] < b_ngh[it_B]) ++it_A;
        else ++it_B;
    }
    a_ngh.clear();
    b_ngh.clear();

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
    auto* g = get_snapshot(G);
    uint64_t n = g->max_nodes();

    std::vector<uint64_t> counts(getWorkers()*8, 0);
    VertexSubset Frontier(0, n, true); //frontier contains all vertices

    edgeMap(g, Frontier, countF<ll_mlcsr_ro_graph>(g, counts), false, false);
    Frontier.del();
    uint64_t count = 0;
    for (int i = 0; i < getWorkers(); i++) {
        count += counts[i*8];
    }
    return count;
}

#endif //EXP_TC_H

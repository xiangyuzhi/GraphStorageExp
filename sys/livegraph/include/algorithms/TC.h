//
// Created by zxy on 5/10/22.
//

#ifndef EXP_TC_H
#define EXP_TC_H
#include "G-map.h"


//assumes sorted neighbor lists
template<typename Trans>
uint64_t countCommon(Trans &tx, uint32_t a, uint32_t b) {

    vector<uint32_t> a_ngh;
    vector<uint32_t> b_ngh;

    for(auto it_A = tx.get_edges(a,0); it_A.valid() ;it_A.next())
        a_ngh.push_back(it_A.dst_id());

    for(auto it_B = tx.get_edges(b,0); it_B.valid() ;it_B.next())
        b_ngh.push_back(it_B.dst_id());

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

template<typename Trans>
struct countF { //for edgeMap
    Trans &tx;
    std::vector<uint64_t> &counts;
    countF(Trans &tx_, std::vector<uint64_t> &_counts) :  tx(tx_), counts(_counts) {}
    inline bool update (uint32_t s, uint32_t d, uint32_t w) {
        if(s > d) {//only count "directed" triangles
            counts[8*getWorkerNum()] += countCommon(tx, s, d);
        }
        return 1;
    }
    inline bool updateAtomic (uint32_t s, uint32_t d, uint32_t w) {
        if (s > d) { //only count "directed" triangles
            counts[8*getWorkerNum()] += countCommon(tx, s, d);
        }
        return 1;
    }
    inline bool cond ([[maybe_unused]] uint32_t d) { return true; } //does nothing
};

template<typename Graph>
uint64_t TC(Graph* G) {
    auto tx = G->begin_read_only_transaction();
    uint64_t n = G->get_max_vertex_id();

    std::vector<uint64_t> counts(getWorkers()*8, 0);
    VertexSubset Frontier(0, n, true); //frontier contains all vertices

    edgeMap(n, tx, Frontier, countF(tx, counts), false);
    Frontier.del();
    uint64_t count = 0;
    for (int i = 0; i < getWorkers(); i++) {
        count += counts[i*8];
    }
    tx.abort();
    return count;
}



#endif //EXP_TC_H

//
// Created by zxy on 5/10/22.
//

#ifndef EXP_TC_H
#define EXP_TC_H
#include "G-map.h"


//assumes sorted neighbor lists
template<typename Trans>
uint64_t countCommon(Trans &tx, uint32_t a, uint32_t b) {

    auto it_A = tx.get_edges(a, /* label */ 0);
    auto it_B = tx.get_edges(b, /* label */ 0);
    long ans=0;
    while (it_A.valid() && it_B.valid() && (it_A.dst_id()) < a && (it_B.dst_id()) < b) { //count "directed" triangles
        if ((it_A.dst_id()) == (it_B.dst_id())) it_A.next(), it_B.next(), ans++;
        else if ((it_A.dst_id()) < (it_B.dst_id())) it_A.next();
        else it_B.next();
    }

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

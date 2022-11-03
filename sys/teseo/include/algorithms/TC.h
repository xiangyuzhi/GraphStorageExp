//
// Created by zxy on 5/10/22.
//

#ifndef EXP_TC_H
#define EXP_TC_H
#include "G-map.h"


//assumes sorted neighbor lists
uint64_t countCommon(uint32_t a, uint32_t b, const uint32_t* ngh, const uint64_t* start) {

    uint64_t ans=0;
    uint32_t it_A = 0, it_B = 0;
    while (it_A!=start[a+1] && it_B!=start[b+1] && ngh[it_A] < a && ngh[it_B] < b) { //count "directed" triangles
        if (ngh[it_A] == ngh[it_B]) ++it_A, ++it_B, ans++;
        else if (ngh[it_A] < ngh[it_B]) ++it_A;
        else ++it_B;
    }

    return ans;
}

struct countF { //for edgeMap
    std::vector<uint64_t> &counts;
    uint32_t *ngh;
    uint64_t *start;
    countF( std::vector<uint64_t> &_counts, uint32_t* _ngh,  uint64_t* _start) :
        counts(_counts), ngh(_ngh),  start(_start){}
    inline bool update (uint32_t s, uint32_t d, uint32_t w) {
        if(s > d) {//only count "directed" triangles
            counts[8*getWorkerNum()] += countCommon(s, d, ngh, start);
        }
        return true;
    }
    inline bool updateAtomic (uint32_t s, uint32_t d, uint32_t w) {
        if (s > d) { //only count "directed" triangles
            counts[8*getWorkerNum()] += countCommon(s, d, ngh, start);
        }
        return true;
    }
    inline bool cond ([[maybe_unused]] uint32_t d) { return true; } //does nothing
};

uint64_t TC(OpenMP &openmp) {
    uint64_t n = openmp.transaction().num_vertices();
    uint64_t m = 0;
    auto* start = newA(uint64_t, n+1);
    for(uint32_t i =0; i<n;i++){
        start[i] = m;
        m+= openmp.transaction().degree(i, /* logical */ false);
    }
    start[n] = m;
    auto* ngh = newA(uint32_t, m);
    uint64_t cnt = 0;

    for (uint32_t i =0; i<n;i++){
        openmp.iterator().edges(i, /* logical ? */ false, [&](uint64_t v){
            ngh[cnt++] = v;
        });
    }

    std::vector<uint64_t> counts(getWorkers()*8, 0);
    VertexSubset Frontier(0, n, true); //frontier contains all vertices

    edgeMap(openmp, Frontier, countF(counts, ngh, start), false, false);
    Frontier.del();
    uint64_t count = 0;
    for (int i = 0; i < getWorkers(); i++) {
        count += counts[i*8];
    }

    return count;
}

#endif //EXP_TC_H

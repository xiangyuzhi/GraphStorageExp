//
// Created by zxy on 5/10/22.
//

#ifndef EXP_TC_H
#define EXP_TC_H
#include "G-map.h"
#include "type.h"

//assumes sorted neighbor lists
template<typename Graph>
uint64_t countCommon(Graph *G, uint32_t a, uint32_t b) {
    uint64_t deg_A = G->get_degree_out(a);
    lite_edge_t* a_neighbours = nullptr;
    a_neighbours = reinterpret_cast<decltype(a_neighbours)>( realloc(a_neighbours, sizeof(a_neighbours[0]) * deg_A));
    G->get_nebrs_out(a, a_neighbours);
    uint64_t* a_ngh = new uint64_t[deg_A];
    for(uint64_t i = 0; i < deg_A; i++)
        a_ngh[i] = get_sid(a_neighbours[i]);

    free(a_neighbours); a_neighbours = nullptr;

    uint64_t deg_B = G->get_degree_out(b);
    lite_edge_t* b_neighbours = nullptr;
    b_neighbours = reinterpret_cast<decltype(b_neighbours)>( realloc(b_neighbours, sizeof(b_neighbours[0]) * deg_B));
    G->get_nebrs_out(b, b_neighbours);
    uint64_t* b_ngh = new uint64_t[deg_B];
    for(uint64_t i = 0; i < deg_B; i++)
        b_ngh[i] = get_sid(b_neighbours[i]);

    free(b_neighbours); b_neighbours = nullptr;

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
            counts[8*getWorkerNum()] += countCommon(G,s,d);
        }
        return 1;
    }
    inline bool updateAtomic (uint32_t s, uint32_t d, uint32_t w) {
        if (s > d) { //only count "directed" triangles
            counts[8*getWorkerNum()] += countCommon(G,s,d);
        }
        return 1;
    }
    inline bool cond ([[maybe_unused]] uint32_t d) { return true; } //does nothing
};

template<typename Graph>
uint64_t TC(Graph* G) {
    size_t n = G->v_count;
    std::vector<uint64_t> counts(getWorkers()*8, 0);
    VertexSubset Frontier(0,n,true); //frontier contains all vertices

    edgeMap(G,Frontier,countF(G,counts), false);
    Frontier.del();
    uint64_t count = 0;
    for (int i = 0; i < getWorkers(); i++) {
        count += counts[i*8];
    }
    return count;
#if VERIFY
    printf("triangle count = %ld\n",count);
#endif
}



#endif //EXP_TC_H

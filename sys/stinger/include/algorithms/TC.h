//
// Created by zxy on 5/10/22.
//

#ifndef EXP_TC_H
#define EXP_TC_H
#include "G-map.h"

struct Edge_iter { //for edgeMap

public:

    uint64_t dst;

    void next(){
        if(!stinger_eb_is_blank(current_eb__, itr)) {
            struct stinger_edge *current_edge__ = current_eb__->edges + itr;
            if (STINGER_IS_OUT_EDGE) {
                dst = STINGER_EDGE_DEST;// do operation
            }
        }
        itr ++;
        if(itr>= stinger_eb_high(current_eb__)){
            current_eb__ = ebpool_priv + (current_eb__->next);
            itr = 0 ;
        }
    }

    Edge_iter(uint64_t vid){
        ebpool_priv = ebpool->ebpool;
        current_eb__ = ebpool_priv + stinger_vertex_edges_get(vertices, vid);
        itr = 0;
        if(current_eb__ != ebpool_priv)
        if(itr< stinger_eb_high(current_eb__))
            if(!stinger_eb_is_blank(current_eb__, itr)) {
                struct stinger_edge *current_edge__ = current_eb__->edges + itr;
                if (STINGER_IS_OUT_EDGE) {
                    dst = STINGER_EDGE_DEST;// do operation
                }
            }
    }

    bool end(){
        return (current_eb__ == ebpool_priv);
    }

private:
    MAP_STING(G);
    struct stinger_eb * ebpool_priv;// = ebpool->ebpool;
    struct stinger_eb *  current_eb__;// = ebpool_priv + stinger_vertex_edges_get(vertices, a);
    uint64_t itr;
};


//assumes sorted neighbor lists
template<typename Graph>
uint64_t countCommon(Graph *G, uint32_t a, uint32_t b, uint64_t *a_ngh, uint64_t *b_ngh) {

    Edge_iter it_A = Edge_iter(a);
    Edge_iter it_B = Edge_iter(b);
    uint64_t ans=0;
    while (!it_A.end() && !it_B.end() && it_A.dst < a && it_B.dst < b) { //count "directed" triangles
        if (it_A.dst == it_B.dst) it_A.next(), it_B.next(), ans++;
        else if (it_A.dst < it_B.dst) it_A.next();
        else it_B.next();
    }

    return ans;
}

template<typename Graph>
struct countF { //for edgeMap
    Graph *G;
    std::vector<uint64_t> &counts;
    uint64_t *a_ngh;
    uint64_t *b_ngh;
    countF(Graph *G_, std::vector<uint64_t> &_counts, uint64_t *a_ngh_, uint64_t *b_ngh_) :
        G(G_), counts(_counts), b_ngh(b_ngh_), a_ngh(a_ngh_) {}
    inline bool update (uint32_t s, uint32_t d, uint32_t w) {
        if(s > d) {//only count "directed" triangles
            counts[8*getWorkerNum()] += countCommon(G, s, d, a_ngh, b_ngh);
        }
        return true;
    }
    inline bool updateAtomic (uint32_t s, uint32_t d, uint32_t w) {
        if (s > d) { //only count "directed" triangles
            counts[8*getWorkerNum()] += countCommon(G, s, d, a_ngh, b_ngh);
        }
        return true;
    }
    inline bool cond ([[maybe_unused]] uint32_t d) { return true; } //does nothing
};

template<typename Graph>
uint64_t TC(Graph* G) {
    uint64_t n = G->max_nv;
    auto *AN = new uint64_t[n];
    auto *BN = new uint64_t[n];
    std::vector<uint64_t> counts(getWorkers()*8, 0);
    VertexSubset Frontier(0, n, true); //frontier contains all vertices

    edgeMap(G, Frontier, countF<Graph>(G, counts, AN, BN), false);
    Frontier.del();
    uint64_t count = 0;
    for (int i = 0; i < getWorkers(); i++) {
        count += counts[i*8];
    }
    return count;
}

#endif //EXP_TC_H

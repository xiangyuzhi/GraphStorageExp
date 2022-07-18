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
uint64_t countCommon(Graph *G, uint32_t a, uint32_t b) {
    vector<uint32_t> a_ngh;
    vector<uint32_t> b_ngh;

    for(Edge_iter it_A = Edge_iter(a); !it_A.end() ;it_A.next())
        a_ngh.push_back(it_A.dst);

    for(Edge_iter it_B = Edge_iter(b); !it_B.end() ;it_B.next())
        b_ngh.push_back(it_B.dst);

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
    countF(Graph *G_, std::vector<uint64_t> &_counts) :
        G(G_), counts(_counts) {}
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
    uint64_t n = G->max_nv;
    std::vector<uint64_t> counts(getWorkers()*8, 0);
    VertexSubset Frontier(0, n, true); //frontier contains all vertices

    edgeMap(G, Frontier, countF<Graph>(G, counts), false);
    Frontier.del();
    uint64_t count = 0;
    for (int i = 0; i < getWorkers(); i++) {
        count += counts[i*8];
    }
    return count;
}

#endif //EXP_TC_H

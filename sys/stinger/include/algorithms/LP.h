//
// Created by zxy on 5/10/22.
//

#ifndef EXP_LP_H
#define EXP_LP_H


#define newA(__E,__n) (__E*) malloc((__n)*sizeof(__E))
#include "G-map.h"
#include "parallel_util.h"
#include <map>

using namespace std;

struct LP_F {
    vector<map<uint32_t, uint32_t> > *histogram;
    uint32_t *label;
    LP_F(vector<map<uint32_t, uint32_t> >* _histogram, uint32_t *_label): histogram(_histogram), label(_label){}
    inline bool update (uint32_t s, uint32_t d, uint32_t w) { //Update ShortestPathLen if found a shorter path
        if ((*histogram)[s][label[d]] == NULL)
            (*histogram)[s][label[d]] = 1;
        else (*histogram)[s][label[d]] += 1;
        return 0;
    }
    inline bool updateAtomic (uint32_t s, uint32_t d, uint32_t w){ //atomic Update=
        return 0;
    }
    inline bool cond (uint32_t d) { return 1; }
};

template<typename T>
struct LP_Vertex {
    T *p_curr;
    uint32_t *label;
    LP_Vertex(T* _p_curr,uint32_t *_label) : p_curr(_p_curr),label(_label) {}

    static bool cmp_value(const pair<uint32_t, uint32_t> left,const pair<uint32_t,uint32_t> right){
        return left.second < right.second;
    }
    inline bool operator() (uint32_t i){
        map<uint32_t ,uint32_t>::iterator it=(*p_curr)[i].begin();
        auto mxitr= max_element((*p_curr)[i].begin(),(*p_curr)[i].end(),LP_Vertex::cmp_value);
        uint32_t mx = mxitr->second;
        while(it != (*p_curr)[i].end()){
            if(it->second == mx){
                label[i] = it->first;
                break ;
            }
            it++;
        }
        return 1;
    }
};


//reset all vertices
template<typename T>
struct LP_Vertex_Reset {
    T* p;
    LP_Vertex_Reset(T* _p) : p(_p) {}
    inline bool operator () (uint32_t i) {
        (*p)[i].clear();
        return 1;
    }
};

template<typename Graph>
void LP(Graph *G, long itr) {
    uint64_t n = G->max_nv;

    auto *hist=  new vector<map<uint32_t ,uint32_t> >(n);
    auto *lb = newA(uint32_t,n);
    parallel_for(size_t i = 0; i < n; i++) {
        lb[i] = rand()%3;
    }
    VertexSubset frontier = VertexSubset(0, n, true);
    while(itr--){
        edgeMap(G, frontier, LP_F(hist, lb), false, false );
        vertexMap(frontier,LP_Vertex<vector<map<uint32_t ,uint32_t> >>(hist, lb), false);
        vertexMap(frontier,LP_Vertex_Reset<vector<map<uint32_t ,uint32_t> >>(hist), false);
    }

    frontier.del();
    free(hist);
    free(lb);
}

#endif //EXP_LP_H

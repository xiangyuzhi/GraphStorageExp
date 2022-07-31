//
// Created by zxy on 5/9/22.
//

#ifndef EXP_LP_H
#define EXP_LP_H

#define newA(__E,__n) (__E*) malloc((__n)*sizeof(__E))
#include "utils/parallel_util.h"
#include <map>
#include <vector>
#define LABEL 15

using namespace std;


struct LP_F {
    uint32_t **lb_cnt;
    uint32_t *label;
    LP_F(uint32_t **_lb_cnt, uint32_t *_label): lb_cnt(_lb_cnt),  label(_label){}
    inline bool update (uint32_t s, uint32_t d) { //Update ShortestPathLen if found a shorter path
        lb_cnt[label[d]][s]++;
        return 0;
    }
    inline bool updateAtomic (uint32_t s, uint32_t d){ //atomic Update=
        return 0;
    }
    inline bool cond (uint32_t d) { return 1; }
};


struct LP_Vertex {
    uint32_t **lb_cnt;
    uint32_t *label;
    LP_Vertex(uint32_t **_lb_cnt, uint32_t *_label) :
            label(_label),lb_cnt(_lb_cnt)  {}

    inline bool operator() (uint32_t i){
        if(i == 0) return 1;
        uint32_t max_lb = 0;
        uint32_t max_cnt = 0;
        for(uint32_t ilb = LABEL; ilb>=1 ; ilb--){
            if(max_cnt <= lb_cnt[ilb][i]){
                max_cnt = lb_cnt[ilb][i];
                max_lb = ilb;
            }
        }
        label[i] = max_lb;
        return 1;
    }
};


//reset all vertices
struct LP_Vertex_Reset {
    uint32_t **lb_cnt;
    LP_Vertex_Reset(uint32_t **_lb_cnt) : lb_cnt(_lb_cnt) {}
    inline bool operator () (uint32_t i) {
        for(uint32_t ilb =1 ; ilb<=LABEL ; ilb++){
            lb_cnt[ilb][i] = 0;
        }
        return 1;
    }
};

template<typename Graph>
void LP(Graph &G, long itr) {
    long n = G.num_vertices();

    auto **lb_cnt = newA(uint32_t*,LABEL+1);
    for (uint32_t i = 0; i <LABEL+1; i++) {
        *(lb_cnt + i) = newA(uint32_t, n);
    }

    auto *lb = newA(uint32_t,n);
    parallel_for(0, n, [&] (size_t i) {
        lb[i] = (i%LABEL+LABEL)%LABEL + 1;
    });

    bool* tt = pbbs::new_array_no_init<bool>(1);
    vertex_subset frontier = vertex_subset(n, n, tt);
    timer sparse_t, dense_t, fetch_t, other_t;
    while(itr--){
        G.edge_map(frontier, LP_F(lb_cnt, lb), sparse_t, dense_t, fetch_t, other_t, stay_dense);

        vertex_map(frontier,LP_Vertex(lb_cnt, lb));
        vertex_map(frontier,LP_Vertex_Reset(lb_cnt));
    }
}


#endif //EXP_LP_H

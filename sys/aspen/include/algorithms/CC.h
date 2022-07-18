//
// Created by zxy on 5/9/22.
//

#ifndef EXP_CC_H
#define EXP_CC_H

#include "utils/parallel_util.h"
#define newA(__E,__n) (__E*) malloc((__n)*sizeof(__E))
#pragma once

struct CC_Shortcut {
    uint32_t* IDs, *prevIDs;
    CC_Shortcut( uint32_t* _IDs,  uint32_t* _prevIDs) :
            IDs(_IDs), prevIDs(_prevIDs) {}
    inline bool operator () ( uint32_t i) {
        uint32_t l = IDs[IDs[i]];
        if(IDs[i] != l) IDs[i] = l;
        if(prevIDs[i] != IDs[i]) {
            prevIDs[i] = IDs[i];
            return 1; }
        else return 0;
    }
};
struct CC_Vertex_F {
    uint32_t* IDs, *prevIDs;
    CC_Vertex_F(uint32_t* _IDs, uint32_t* _prevIDs) :
            IDs(_IDs), prevIDs(_prevIDs) {}
    inline bool operator () (uint32_t i) {
        prevIDs[i] = IDs[i];
        return 1; }};



struct CC_F {
    uint32_t* IDs, *prevIDs;
    CC_F( uint32_t* _IDs,  uint32_t* _prevIDs) :
            IDs(_IDs), prevIDs(_prevIDs) {}
    inline bool update( uint32_t s,  uint32_t d){ //Update function writes min ID
        uint32_t origID = IDs[d];
        if(IDs[s] < origID) {
            IDs[d] = IDs[s];
            if(origID == prevIDs[d]) return 1;
        } return 0; }
    inline bool updateAtomic ( uint32_t s,  uint32_t d) { //atomic Update
        uint32_t origID = IDs[d];
        return (writeMin(&IDs[d],IDs[s]) && origID == prevIDs[d]);
    }
    inline bool cond ([[maybe_unused]] uint32_t d) { return true; } //does nothing
};

template <typename Graph>
uint32_t *CC(Graph &G) {
    long n = G.num_vertices();
    uint32_t* IDs = newA( uint32_t,n), *prevIDs = newA( uint32_t,n);
    //initialize unique IDs
    for(long i=0;i<n;i++) {
        IDs[i] = i;
    }

    bool* tt = pbbs::new_array_no_init<bool>(1);
    vertex_subset Active = vertex_subset(n,n,tt); //initial frontier contains all vertices

    timer sparse_t, dense_t, fetch_t, other_t;
    while(!Active.is_empty()){ //iterate until IDS converge
        vertex_map(Active, CC_Vertex_F(IDs,prevIDs));
        vertex_subset next = G.edge_map(Active, CC_F(IDs,prevIDs), sparse_t, dense_t, fetch_t, other_t, stay_dense);
        Active.del();
        Active = next;
    }
    Active.del();
    free(prevIDs);

#if VERIFY
    std::set<uint32_t> components_set;
  for (uint32_t i = 0; i < n; i++) {
    components_set.insert(IDs[i]);
  }
  printf("number of components is %lu\n", components_set.size());
#endif

    return IDs;
}


#endif //EXP_CC_H

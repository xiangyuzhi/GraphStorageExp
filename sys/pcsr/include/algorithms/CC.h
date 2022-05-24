//
// Created by zxy on 5/10/22.
//

#ifndef EXP_CC_H
#define EXP_CC_H

#include "G-map.h"
#include "parallel_util.h"

#define newA(__E,__n) (__E*) malloc((__n)*sizeof(__E))

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
    inline bool update( uint32_t s,  uint32_t d, uint32_t w){ //Update function writes min ID
        uint32_t origID = IDs[d];
        if(IDs[s] < origID) {
            IDs[d] = IDs[s];
            if(origID == prevIDs[d]) return 1;
        } return 0; }
    inline bool updateAtomic ( uint32_t s,  uint32_t d, uint32_t w) { //atomic Update
        uint32_t origID = IDs[d];
        return (writeMin(&IDs[d],IDs[s]) && origID == prevIDs[d]);
    }
    inline bool cond ([[maybe_unused]] uint32_t d) { return true; } //does nothing
};

template<typename Graph>
void CC(Graph *G) {
    uint64_t n = G->get_n();
    auto* IDs = newA( uint32_t,n), *prevIDs = newA( uint32_t,n);

    parallel_for(long i=0;i<n;i++) { //initialize unique IDs
        IDs[i] = i;
    }

    VertexSubset Active = VertexSubset(0, n, true); //initial frontier contains all vertices

    while(Active.not_empty()){ //iterate until IDS converge
        vertexMap(Active,CC_Vertex_F(IDs,prevIDs), false);
        VertexSubset next = edgeMap(G, Active, CC_F(IDs,prevIDs),true, true); //INT_MAX);
        Active.del();
        Active = next;
    }

    Active.del();
    free(prevIDs);
}

#endif //EXP_CC_H

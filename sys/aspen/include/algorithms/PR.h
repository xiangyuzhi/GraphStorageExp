//
// Created by zxy on 5/9/22.
//

#ifndef EXP_PR_H
#define EXP_PR_H


#include "math.h"
#pragma once
#define newA(__E,__n) (__E*) malloc((__n)*sizeof(__E))

// template <class vertex>
template<typename T>
struct PR_F {
    T* p_curr, *p_next;
    PR_F(T* _p_curr, T* _p_next) :
            p_curr(_p_curr), p_next(_p_next) {}
    inline bool update(uint32_t s, uint32_t d){ //update function applies PageRank equation
        p_next[d] += p_curr[s];
        return 1;
    }
    inline bool updateAtomic ([[maybe_unused]] uint32_t s, [[maybe_unused]] uint32_t d) { //atomic Update
        printf("should never be called for now since its always dense\n");
        while(1) { }
    }
    inline bool cond ([[maybe_unused]] uint32_t  d) { return 1; }}; // from ligra readme: for cond which always ret true, ret cond_true// return cond_true(d); }};

template<typename T,typename Graph>
struct PR_Vertex {
    T *p_curr;
    Graph& G;
    PR_Vertex(T* _p_curr, Graph& _G) : p_curr(_p_curr), G(_G) {}
    inline bool operator () (uint32_t i) {
        p_curr[i] = p_curr[i] /G.find_vertex(i).value.degree();// G.num_neighbors(i);; // damping*p_next[i] + addedConstant;
        return 1;
    }
};


//resets p
template<typename T>
struct PR_Vertex_Reset {
    T* p;
    PR_Vertex_Reset(T* _p) :
            p(_p) {}
    inline bool operator () (uint32_t i) {
        p[i] = 0.0;
        return 1;
    }
};

template<typename T,typename Graph>
T* PR(Graph& G, long maxIters) {
    size_t n = G.num_vertices();

    T one_over_n = 1/(double)n;
    T* p_curr = (T*) memalign(32, n*sizeof(T));
    T* p_next = (T*) memalign(32, n*sizeof(T));
    for(uint64_t i = 0; i < n; i++) {
        p_curr[i] = one_over_n;
    }
    bool* tt = pbbs::new_array_no_init<bool>(1);
    vertex_subset Frontier = vertex_subset(n,n,tt);
    Frontier.to_dense();
    timer sparse_t, dense_t, fetch_t, other_t;
    // VertexSubset Frontier = VertexSubset((uint32_t)0, n);

    long iter = 0;
    printf("max iters %lu\n", maxIters);
    while(iter++ < maxIters) {
        //printf("\t running iteration %lu\n", iter);
        // using flat snapshot
        vertex_map(Frontier, PR_Vertex<T,Graph>(p_curr, G));
        vertex_map(Frontier, PR_Vertex_Reset<T>(p_next));

        G.edge_map(Frontier, PR_F<T>(p_curr, p_next), sparse_t, dense_t, fetch_t, other_t, stay_dense);

        swap(p_curr,p_next);
    }

#if VERIFY
    std::ofstream ofile;
  ofile.open("p_curr_s.out");
  for(uint32_t i = 0; i < n; i++) {
    ofile << p_curr[i] << "\n";
  }
  ofile.close();

	std::ofstream ofile2;
  ofile2.open("p_next_s.out");
  for(uint32_t i = 0; i < n; i++) {
    ofile2 << p_next[i] << "\n";
  }
  ofile2.close();
#endif
    free(p_next);

//    double sum = 0;
//    for(uint32_t i =0 ;i< 7;i++){
//        sum += p_curr[i];
//        printf("%.9f ",p_curr[i]);
//    }
//    cout<<"final : "<< sum <<endl;
    // printf("p curr %p, p next %p\n", p_curr, p_next);
    return p_curr;
}

#endif //EXP_PR_H

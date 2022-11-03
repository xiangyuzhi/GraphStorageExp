// This code is part of the project "Ligra: A Lightweight Graph Processing
// Framework for Shared Memory", presented at Principles and Practice of 
// Parallel Programming, 2013.
// Copyright (c) 2013 Julian Shun and Guy Blelloch
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights (to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// #include "ligra.h"

#pragma once
#include "G-map.h"
#include "malloc.h"

#define newA(__E,__n) (__E*) malloc((__n)*sizeof(__E))

// template <class vertex>
template<typename T>
struct PR_F {
  T* p_curr, *p_next;

  PR_F(T* _p_curr, T* _p_next) :
    p_curr(_p_curr), p_next(_p_next) {}
  inline bool update(uint32_t s, uint32_t d, int32_t w){ //update function applies PageRank equation
    p_next[d] += p_curr[s];
    return 1;
  }
  inline bool updateAtomic ([[maybe_unused]] uint32_t s, [[maybe_unused]] uint32_t d,[[maybe_unused]] int32_t w) { //atomic Update
    printf("should never be called for now since its always dense\n");
    while(1) { }
  }
  inline bool cond ([[maybe_unused]] uint32_t  d) { return 1; }}; // from ligra readme: for cond which always ret true, ret cond_true// return cond_true(d); }};

template<typename T,typename Graph>
struct PR_Vertex {
  T *p_curr;
  Graph* G;
  PR_Vertex(T* _p_curr, Graph* _G) : p_curr(_p_curr), G(_G) {}
  inline bool operator () (uint32_t i) {
    p_curr[i] = p_curr[i] /G->get_degree_out(i);// G.num_neighbors(i);; // damping*p_next[i] + addedConstant;
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
/*
template <class OT, class intT>
OT plusReduce(OT* A, intT n) {
  return reduce<OT>((intT)0,n,addF<OT>(),getA<OT,intT>(A));
}
*/
template<typename T,typename Graph>
void PR(Graph* G, long maxIters) {
  size_t n = G->v_count;
  //const double damping = 0.85, epsilon = 0.0000001;

  T one_over_n = 1/(double)n;
  T* p_curr = (T*) memalign(32, n*sizeof(T));
  T* p_next = (T*) memalign(32, n*sizeof(T));
  parallel_for(size_t i = 0; i < n; i++) {
    p_curr[i] = one_over_n;
  }
  VertexSubset Frontier = VertexSubset(0, n, true);
  
  long iter = 0;
  printf("max iters %lu\n", maxIters);
  while(iter++ < maxIters) {
    //printf("\t running iteration %lu\n", iter); // using flat snapshot
    vertexMap(Frontier,PR_Vertex<T,Graph>(p_curr, G), false);
    vertexMap(Frontier,PR_Vertex_Reset<T>(p_next), false);
    edgeMap(G, Frontier,PR_F<T>(p_curr,p_next),false, false);

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
  Frontier.del();
  free(p_next);
  free(p_curr);
  // printf("p curr %p, p next %p\n", p_curr, p_next);
}

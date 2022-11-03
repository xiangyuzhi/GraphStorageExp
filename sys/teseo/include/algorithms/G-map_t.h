//
// Created by zxy on 5/9/22.
//
#ifndef EXP_G_MAP_H
#define EXP_G_MAP_H
#include "G-travese_t.h"
#include "vertex-subset.h"

using namespace graph_traversal;

template<class F, class Graph>
struct EDGE_MAP_SPARSE {
    VertexSubset &output_vs;
    Graph &G;
    F f;
    bool weighted;
    bool output;
    EDGE_MAP_SPARSE(Graph &G_, VertexSubset &output_vs_, F f_, bool output_, bool weighted) :
            G(G_), output_vs(output_vs_), f(f_), output(output_), weighted(weighted) {}
    inline bool update(uint32_t val) {
        G.register_thread();
        //printf("spare from %d\n", omp_get_thread_num());
        auto tx = G.start_transaction(true);
        map_sparse(tx, f, output_vs, val, output, weighted);

        G.unregister_thread();
        //printf("spare unreg from %d\n", omp_get_thread_num());
        return false;
    }
};

template <class F, class Graph>
VertexSubset EdgeMapSparse(Graph &G, VertexSubset &vs, F f, bool output, bool weighted) {
    vs.convert_to_sparse();
    VertexSubset output_vs = VertexSubset(vs, false);
    struct EDGE_MAP_SPARSE<F, Graph> v(G, output_vs, f, output, weighted);
    vs.map(v);
    output_vs.finalize();
    return output_vs;
}


template <class F, class Graph>
VertexSubset EdgeMapDense(Graph &G, VertexSubset &vs, F f, bool output, bool weighted) {
    vs.convert_to_dense();
    G.register_thread();
    auto tx = G.start_transaction(true);
    uint64_t n = tx.num_vertices();
    G.unregister_thread();
    VertexSubset output_vs = VertexSubset(vs, false);
    if (vs.all) {
        parallel_for(uint64_t i_ = 0; i_ < n; i_+=512) {
            uint64_t end = std::min(i_+512, (uint64_t)n );
            G.register_thread();
            //auto tx = G.start_transaction(true);
            for (uint64_t i = i_; i < end; i++) {
                if (f.cond(i) == 1) { //printf("processing row %lu\n", i);
                    map_dense_vs_all(G, f, vs, output_vs, i, output, weighted);
                }
            }
            G.unregister_thread();
        }
    } else {
        parallel_for(uint64_t i_ = 0; i_ < n; i_+=512) {
            uint64_t end = std::min(i_+512, (uint64_t) n);
            G.register_thread();
            //auto tx = G.start_transaction(true);
            for (uint64_t i = i_; i < end; i++) {
                if (f.cond(i) == 1) {//printf("processing row %lu\n", i);
                    map_dense_vs_not_all(G, f, vs, output_vs, i, output, weighted);
                }
            }
            G.unregister_thread();
        }
    }
    return output_vs;

}

template <class F, class Graph>
VertexSubset edgeMap(Graph &G, VertexSubset &vs, F f, bool weighted = false, bool output = true, uint32_t threshold = 20) {
    G.register_thread();
    auto tx = G.start_transaction(true);
    uint64_t n = tx.num_vertices();
    G.unregister_thread();
    if (n/threshold <= vs.get_n()) {
        return EdgeMapDense(G, vs, f, output, weighted);
    } else {
        return EdgeMapSparse(G, vs, f, output, weighted);
    }
}

template<class F, bool output>
struct VERTEX_MAP {
    VertexSubset &output_vs;
    F f;
    VERTEX_MAP(VertexSubset &output_vs_, F f_) : output_vs(output_vs_), f(f_) {}
    inline bool update(uint32_t val) {
        if constexpr (output) {
            if (f(val) == 1) {
                output_vs.insert(val);
            }
        } else {
            f(val);
        }
        return false;
    }
};

template <class F>
inline VertexSubset vertexMap(VertexSubset &vs, F f, bool output = true) {
    //TODO the compilier should have been able to do this itself
    if (output) {
        VertexSubset output_vs = VertexSubset(vs, false);
        struct VERTEX_MAP<F, true> v(output_vs, f);
        vs.map(v);
        output_vs.finalize();
        return output_vs;
    } else {
        VertexSubset null_vs = VertexSubset();
        struct VERTEX_MAP<F, false> v(null_vs, f);
        vs.map(v);
        return null_vs;
    }
}

#endif //EXP_G_MAP_H
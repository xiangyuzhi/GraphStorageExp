//
// Created by zxy on 5/9/22.
//

#ifndef EXP_G_TRAVESE_H
#define EXP_G_TRAVESE_H


namespace graph_traversal {

    template <class F, class Graph, class VS>
    void map_sparse(Graph *G, F f,VS &output_vs, uint32_t self_index, bool output, bool weighted){

        ll_edge_iterator iter;
        G->out_iter_begin(iter, self_index);
        for (edge_t s_idx = G->out_iter_next(iter); s_idx != LL_NIL_EDGE; s_idx = G->out_iter_next(iter)) {
            node_t v = LL_ITER_OUT_NEXT_NODE(graph, iter, s_idx);
            uint32_t w = 0;
            if (weighted) w = (uint32_t)get_out_edge_weight(G, s_idx);
            if (f.cond(v) == 1 && f.updateAtomic(self_index, v, w) == 1) {
                if (output) {
                    output_vs.insert_sparse(v);
                }
            }
        }
    }

//    template<typename Graph>
//    static uint64_t get_out_edge_weight(Graph* graph, edge_t edge_id){
//        return reinterpret_cast<ll_mlcsr_edge_property<uint64_t>*>(graph->get_edge_property_64(g_llama_property_weights))->get(edge_id);
//    }

    template <class F, class Graph, class VS>
    void map_dense_vs_all(Graph *G, F f,VS &vs, VS &output_vs, int64_t self_index, bool output, bool weighted){

        ll_edge_iterator iter;
        G->out_iter_begin(iter, self_index);
        for (edge_t s_idx = G->out_iter_next(iter); s_idx != LL_NIL_EDGE; s_idx = G->out_iter_next(iter)) {
            node_t v = LL_ITER_OUT_NEXT_NODE(G, iter, s_idx);
            uint32_t w = 0;
            if (weighted) w = (uint32_t)get_out_edge_weight(G, s_idx);
            if (f.update(v, self_index, w) == 1) {
                if (output) {
                    output_vs.insert_dense(self_index);
                }
            }
            if (f.cond(self_index) == 0) {
                return;
            }
        }
    }

    template <class F, class Graph, class VS>
    void map_dense_vs_not_all(Graph *G, F f, VS &vs, VS &output_vs, int64_t self_index, bool output, bool weighted){

        ll_edge_iterator iter;
        G->out_iter_begin(iter, self_index);
        for (edge_t s_idx = G->out_iter_next(iter); s_idx != LL_NIL_EDGE; s_idx = G->out_iter_next(iter)) {
            node_t v = LL_ITER_OUT_NEXT_NODE(graph, iter, s_idx);
            uint32_t w = 0;
            if (weighted) w = (uint32_t)get_out_edge_weight(G, s_idx);
            if (vs.has_dense_no_all(v) && f.update(v, self_index, w) == 1) {
                if (output) {
                    output_vs.insert_dense(self_index);
                }
            }
            if (f.cond(self_index) == 0) {
                return;
            }
        }
    }

}

#endif //EXP_G_TRAVESE_H

//
// Created by zxy on 5/9/22.
//

#ifndef EXP_G_TRAVESE_H
#define EXP_G_TRAVESE_H


namespace graph_traversal {

    template <class F, class Graph, class VS>
    void map_sparse(Graph *G, F f,VS &output_vs, uint32_t self_index, bool output, bool weighted){

        const auto u_interval = G->traverse(self_index);
        auto *out_e = G->out_e();
        auto *out_w = G->out_w();
        for(uint64_t i = u_interval.first; i < u_interval.second; i++){
            uint64_t v = out_e[i];
            uint32_t w = 0;
            if (weighted) w = (uint32_t)out_w[i];
            if (f.cond(v) == 1 && f.updateAtomic(self_index, v, w) == 1) {
                if (output) {
                    output_vs.insert_sparse(v);
                }
            }
        }
    }


    template <class F, class Graph, class VS>
    void map_dense_vs_all(Graph *G, F f,VS &vs, VS &output_vs, int64_t self_index, bool output, bool weighted){

        const auto u_interval = G->traverse(self_index);
        auto *out_e = G->out_e();
        auto *out_w = G->out_w();
        for(uint64_t i = u_interval.first; i < u_interval.second; i++){
            uint64_t v = out_e[i];
            uint32_t w = 0;
            if (weighted) w = (uint32_t)out_w[i];
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

        const auto u_interval = G->traverse(self_index);
        auto *out_e = G->out_e();
        auto *out_w = G->out_w();
        for(uint64_t i = u_interval.first; i < u_interval.second; i++){
            uint64_t v = out_e[i];
            uint32_t w = 0;
            if (weighted) w = (uint32_t)out_w[i];
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

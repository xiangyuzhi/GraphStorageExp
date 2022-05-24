//
// Created by zxy on 5/9/22.
//

#ifndef EXP_G_TRAVESE_H
#define EXP_G_TRAVESE_H


namespace graph_traversal {

    template <class F, class Graph, class VS>
    void map_sparse(Graph *G, F f,VS &output_vs, uint32_t self_index, bool output, bool weighted){

        uint32_t sta = G->nodes[self_index].beginning;
        uint32_t end = G->nodes[self_index].end;

        for (uint32_t i = sta + 1; i < end; i++) {
            uint32_t v = G->edges.items[i].dest;
            uint32_t w = 0;
            if (weighted) w = G->edges.items[i].value;
            if (f.cond(v) == 1 && f.updateAtomic(self_index, v, w) == 1) {
                if (output) {
                    output_vs.insert_sparse(v);
                }
            }
        }
    }


    template <class F, class Graph, class VS>
    void map_dense_vs_all(Graph *G, F f,VS &vs, VS &output_vs, int64_t self_index, bool output, bool weighted){
        uint32_t sta = G->nodes[self_index].beginning;
        uint32_t end = G->nodes[self_index].end;

        for (uint32_t i = sta + 1; i < end; i++) {
            uint32_t v = G->edges.items[i].dest;
            uint32_t w = 0;
            if (weighted) w = G->edges.items[i].value;
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

        uint32_t sta = G->nodes[self_index].beginning;
        uint32_t end = G->nodes[self_index].end;

        for (uint32_t i = sta + 1; i < end; i++) {
            uint32_t v = G->edges.items[i].dest;
            uint32_t w = 0;
            if (weighted) w = G->edges.items[i].value;
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

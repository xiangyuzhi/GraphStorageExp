//
// Created by zxy on 5/9/22.
//

#ifndef EXP_G_TRAVESE_H
#define EXP_G_TRAVESE_H


namespace graph_traversal {

    template<class F,class Graph, typename VS>
    void map_sparse(Graph *G, F f,VS &output_vs, uint32_t self_index, bool output, bool weighted){
        uint64_t degree = G->get_degree_out(self_index);

        lite_edge_t* neighbours = nullptr;
        neighbours = (lite_edge_t*) realloc(neighbours, sizeof(neighbours[0]) * degree);
        degree = G->get_nebrs_out(self_index, neighbours);

        for(uint32_t local_idx = 0; local_idx < degree; local_idx++){
            uint64_t v = get_sid(neighbours[local_idx]);
            uint32_t w = 0;
            if (weighted) w = neighbours[local_idx].second.value32b;
            if (f.cond(v) == 1 && f.updateAtomic(self_index, v, w) == 1) {
                if (output) {
                    output_vs.insert_sparse(v);
                }
            }
        }
        free(neighbours); neighbours = nullptr;
    }


    template <class F,class Graph, typename VS>
    void map_dense_vs_all(Graph *G,F f,VS &vs, VS &output_vs, int64_t self_index, bool output, bool weighted){
        uint64_t degree = G->get_degree_out(self_index);

        lite_edge_t* neighbours = nullptr;
        neighbours = (lite_edge_t*) realloc(neighbours, sizeof(neighbours[0]) * degree);
        degree = G->get_nebrs_out(self_index, neighbours);

        for(uint32_t local_idx = 0; local_idx < degree; local_idx++){
            uint64_t v = get_sid(neighbours[local_idx]);
            uint32_t w = 0;
            if (weighted) w = neighbours[local_idx].second.value32b;
            if (f.update(v, self_index, w) == 1) {
                if (output) {
                    output_vs.insert_dense(self_index);
                }
            }
            if (f.cond(self_index) == 0) {
                return;
            }
        }
        free(neighbours); neighbours = nullptr;
    }

    template <class F,class Graph, typename VS>
    void map_dense_vs_not_all(Graph *G,F f,VS &vs, VS &output_vs, int64_t self_index, bool output, bool weighted){
        uint64_t degree = G->get_degree_out(self_index);

        lite_edge_t* neighbours = nullptr;
        neighbours = (lite_edge_t*) realloc(neighbours, sizeof(neighbours[0]) * degree);
        degree = G->get_nebrs_out(self_index, neighbours);

        for(uint32_t local_idx = 0; local_idx < degree; local_idx++){
            uint64_t v = get_sid(neighbours[local_idx]);
            uint32_t w = 0;
            if (weighted) w = neighbours[local_idx].second.value32b;
            if (vs.has_dense_no_all(v) && f.update(v, self_index, w) == 1) {
                if (output) {
                    output_vs.insert_dense(self_index);
                }
            }
            if (f.cond(self_index) == 0) {
                return;
            }
        }
        free(neighbours); neighbours = nullptr;
    }

}

#endif //EXP_G_TRAVESE_H

//
// Created by zxy on 5/9/22.
//

#ifndef EXP_G_TRAVESE_H
#define EXP_G_TRAVESE_H


namespace graph_traversal {

    template<class F, class Trans, class VS>
    void map_sparse(Trans& tx, F f,VS &output_vs, uint32_t self_index, bool output, bool weighted){
        auto iterator = tx.get_edges(self_index, /* label */ 0);
        while(iterator.valid()){
            uint64_t v = iterator.dst_id();
            uint32_t w = 0;
            if (weighted) w = *reinterpret_cast<const uint32_t*>(iterator.edge_data().data());
            if (f.cond(v) == 1 && f.updateAtomic(self_index, v, w) == 1) {
                if (output) {
                    output_vs.insert_sparse(v);
                }
            }
            iterator.next();
        }

    }


    template <class F, class Trans, class VS>
    void map_dense_vs_all(Trans& tx, F f,VS &vs, VS &output_vs, int64_t self_index, bool output, bool weighted){

        auto iterator = tx.get_edges(self_index, /* label */ 0);
        while(iterator.valid()){
            uint64_t v = iterator.dst_id();
            uint32_t w = 0;
            if (weighted) w = *reinterpret_cast<const uint32_t*>(iterator.edge_data().data());
            if (f.update(v, self_index, w) == 1) {
                if (output) {
                    output_vs.insert_dense(self_index);
                }
            }
            if (f.cond(self_index) == 0) {
                return;
            }
            iterator.next();
        }
    }

    template <class F, class Trans, class VS>
    void map_dense_vs_not_all(Trans& tx, F f, VS &vs, VS &output_vs, int64_t self_index, bool output, bool weighted){

        auto iterator = tx.get_edges(self_index, /* label */ 0);
        while(iterator.valid()){
            uint64_t v = iterator.dst_id();
            uint32_t w = 0;
            if (weighted) w = *reinterpret_cast<const uint32_t *>(iterator.edge_data().data());
            if (vs.has_dense_no_all(v) && f.update(v, self_index, w) == 1) {
                if (output) {
                    output_vs.insert_dense(self_index);
                }
            }
            if (f.cond(self_index) == 0) {
                return;
            }
            iterator.next();
        }
    }

}

#endif //EXP_G_TRAVESE_H

//
// Created by zxy on 5/9/22.
//

#ifndef EXP_G_TRAVESE_H
#define EXP_G_TRAVESE_H


namespace graph_traversal {

    template <class F, class VS>
    void map_sparse(OpenMP &omp, F f,VS &output_vs, uint32_t self_index, bool output, bool weighted){
        if(weighted){
            omp.iterator().edges(self_index, /* logical ? */ false, [&](uint64_t v, double wgh){
                uint32_t w = (uint32_t)wgh;
                if (f.cond(v) == 1 && f.updateAtomic(self_index, v, w) == 1) {
                    if (output) {
                        output_vs.insert_sparse(v);
                    }
                }
            });
        }
        else {
            omp.iterator().edges(self_index, /* logical ? */ false, [&](uint64_t v){
                uint32_t w = 0;
                if (f.cond(v) == 1 && f.updateAtomic(self_index, v, w) == 1) {
                    if (output) {
                        output_vs.insert_sparse(v);
                    }
                }
            });
        }
    }


    template <class F, class VS>
    void map_dense_vs_all(OpenMP &omp, F f,VS &vs, VS &output_vs, int64_t self_index, bool output, bool weighted){
        if(weighted){
            omp.iterator().edges(self_index, /* logical ? */ false, [&](uint64_t v, double wgh){
                uint32_t w = (uint32_t)wgh;
                if (f.update(v, self_index, w) == 1) {
                    if (output) {
                        output_vs.insert_dense(self_index);
                    }
                }
                if (f.cond(self_index) == 0) {
                    return;
                }
            });
        }
        else {
            omp.iterator().edges(self_index, /* logical ? */ false, [&](uint64_t v){
                uint32_t w = 0;
                if (f.update(v, self_index, w) == 1) {
                    if (output) {
                        output_vs.insert_dense(self_index);
                    }
                }
                if (f.cond(self_index) == 0) {
                    return;
                }
            });
        }

    }

    template <class F, class VS>
    void map_dense_vs_not_all(OpenMP &omp, F f, VS &vs, VS &output_vs, int64_t self_index, bool output, bool weighted){

        if(weighted){
            omp.iterator().edges(self_index, /* logical ? */ false, [&](uint64_t v, double wgh){
                uint32_t w = (uint32_t)wgh;
                if (vs.has_dense_no_all(v) && f.update(v, self_index, w) == 1) {
                    if (output) {
                        output_vs.insert_dense(self_index);
                    }
                }
                if (f.cond(self_index) == 0) {
                    return;
                }
            });
        }
        else{
            omp.iterator().edges(self_index, /* logical ? */ false, [&](uint64_t v){
                uint32_t w = 0;
                if (vs.has_dense_no_all(v) && f.update(v, self_index, w) == 1) {
                    if (output) {
                        output_vs.insert_dense(self_index);
                    }
                }
                if (f.cond(self_index) == 0) {
                    return;
                }
            });
        }

    }

}

#endif //EXP_G_TRAVESE_H

//
// Created by zxy on 4/14/22.
//


#include<iostream>
#include "llama_test.h"
using namespace std;





int main(int argc, char** argv){
//    char* database_directory = (char*) alloca(16);
//    strcpy(database_directory, "db");
//
//    ll_database tstdb(database_directory);
//    auto& csr = tstdb.graph()->ro_graph();
//    csr.create_uninitialized_edge_property_64(g_llama_property_weights, LL_T_DOUBLE);
//    ll_writable_graph& tstG = *tstdb.graph();
//
//    tstG.tx_begin();
//    for (uint32_t i =1 ; i< 30;i++){
//        edge_t edge_id = tstG.add_edge(0, i);
//        uint64_t w = 1;
//        tstG.get_edge_property_64(g_llama_property_weights)->set(edge_id, *reinterpret_cast<uint64_t*>(&(w)));
//
//    }
//    tstG.checkpoint();
//    tstG.checkpoint();
//    tstG.tx_commit();

    commandLine P(argc, argv, "./test_parallel [-t testname -r rounds -f file -m (mmap)]");
    load_graph(P);

    ll_mlcsr_ro_graph* tt = get_snapshot(G);
//    ll_mlcsr_ro_graph &ttG  = *tt;

    printf("max node: %u\n",tt->max_nodes());
    ll_edge_iterator iter;
//    for(uint32_t i = 0;i < ttG.max_nodes(); i++){
        tt->out_iter_begin(iter, 1);
        for (edge_t s_idx = tt->out_iter_next(iter); s_idx != LL_NIL_EDGE; s_idx = tt->out_iter_next(iter)) {
            node_t v = LL_ITER_OUT_NEXT_NODE(ttG, iter, s_idx);
            uint32_t w = 0 ;
            w = (uint32_t) get_out_edge_weight(tt, s_idx);
            printf("v = %d w = %d\n", v, w);
            //printf("w :%d\n", w);
        }
//    }
    free(tt);
//    G.tx_begin();
//    G.add_edge(1, 2);
//    cout << "number of edges: " << G.max_edges(G.num_levels()) << " Out-degree of 1: " << G.out_degree(1);
//    G.delete_edge(1, G.find(1, 2));
//    cout << "number of edges: " << G.max_edges(G.num_levels()) << " Out-degree of 1: " << G.out_degree(1);
    return 0;
}




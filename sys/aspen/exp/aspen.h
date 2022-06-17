//
// Created by 15743 on 2022/6/9.
//

#ifndef EXP_ASPEN_H
#define EXP_ASPEN_H
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cmath>
#include <sys/mman.h>
#include <stdio.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include "sys/time.h"

#include "pbbslib/strings/string_basics.h"
#include "pbbslib/sequence_ops.h"
#include "pbbslib/monoid.h"

#include "graph/versioned_graph.h"
#include "common/compression.h"
#include "graph/api.h"



struct aspen{
    typedef unsigned long uintE;
    typedef unsigned int uintV;
    using treeplus_graph = traversable_graph<sym_immutable_graph_tree_plus>;

    versioned_graph<treeplus_graph> *G ;

    uint32_t num_vertices(){
        auto S = G->acquire_version();
        uint32_t n = S.graph.num_vertices();
        G->release_version(std::move(S));
        return n;
    }

    void traverse_vertex(uint32_t vsize){
        auto S = G->acquire_version();
        for (uint32_t i = 0; i < vsize; i++)
            auto a = S.graph.find_vertex(i);
        G->release_version(std::move(S));
    }

    void load_graph(commandLine& P){
        G = new versioned_graph(initialize_treeplus_graph(P));
    }

    void init_graph(uint32_t VN){
        size_t n = VN;
        size_t m = 0;
        uintE* offsets = pbbs::new_array_no_init<uintE>(n);
        uintV* edges = pbbs::new_array_no_init<uintV>(m);
        std::tie(n, m, offsets, edges) =  make_tuple(n, m, offsets, edges);
        G = new versioned_graph<treeplus_graph>(n, m, offsets, edges);
    }

    void del(){
        free(G);G = nullptr;
    }

    void add_edges(uint32_t updates_to_run,std::pair<uint64_t, uint64_t> *raw_edges, uint nn){
        G->insert_edges_batch(updates_to_run, reinterpret_cast<tuple<uintV, uintV> *>(raw_edges), false, true, nn, false);
    }

    template <class Graph>
    bool find_e(Graph& G, uintV src, uintV dst) {
        const auto& v = G.find_vertex(src).value;
        bool found = false;
        auto map_f = [&] (uintV ngh_id, size_t ind) {
            if (dst == ngh_id) {
                found = true;
            }
        };
        v.map_elms(src, map_f);
        return found;
    }

    void read_edges(uint32_t updates_to_run,std::pair<uint64_t, uint64_t> *raw_edges){
        auto S = G->acquire_version();
        for (uint32_t i =0 ; i< updates_to_run;i++){
            find_e(S.graph,get<0>(raw_edges[i]),get<1>(raw_edges[i]));
        }
        G->release_version(std::move(S));
    }

    void del_edges(uint32_t updates_to_run,std::pair<uint64_t, uint64_t> *raw_edges, uint nn){
        G->delete_edges_batch(updates_to_run, reinterpret_cast<tuple<uintV, uintV> *>(raw_edges), false, true, nn, false);
    }

    struct Edge_iter { //for edgeMap
    public:
        uint64_t dst;
        uint32_t wgh;

        void next(){
            if(!stinger_eb_is_blank(current_eb__, itr)) {
                struct stinger_edge *current_edge__ = current_eb__->edges + itr;
                if (STINGER_IS_OUT_EDGE) {
                    dst = STINGER_EDGE_DEST;// do operation
                }
            }
            itr ++;
            if(itr>= stinger_eb_high(current_eb__)){
                current_eb__ = ebpool_priv + (current_eb__->next);
                itr = 0 ;
            }
        }

        Edge_iter(uint64_t vid){
            ebpool_priv = ebpool->ebpool;
            current_eb__ = ebpool_priv + stinger_vertex_edges_get(vertices, vid);
            itr = 0;
            if(current_eb__ != ebpool_priv)
                if(itr< stinger_eb_high(current_eb__))
                    if(!stinger_eb_is_blank(current_eb__, itr)) {
                        struct stinger_edge *current_edge__ = current_eb__->edges + itr;
                        if (STINGER_IS_OUT_EDGE) {
                            dst = STINGER_EDGE_DEST;// do operation
                        }
                    }
        }

        bool end(){
            return (current_eb__ == ebpool_priv);
        }

    private:
        MAP_STING(G);
        struct stinger_eb * ebpool_priv;// = ebpool->ebpool;
        struct stinger_eb *  current_eb__;// = ebpool_priv + stinger_vertex_edges_get(vertices, a);
        uint64_t itr;
    };

};




#endif //EXP_ASPEN_H

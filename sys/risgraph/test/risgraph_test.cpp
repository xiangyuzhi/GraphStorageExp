//
// Created by zxy on 4/16/22.
//

#include "risgraph_test.h"
#include "utils/io_util.h"
#include "utils/rmat_util.h"

void load_graph(commandLine& P){
    auto filename = P.getOptionValue("-f", "none");
    pair_uint *edges = get_edges_from_file_adj_sym(filename.c_str(), &num_edges, &num_nodes);


    G = new Graph<uint64_t> (num_nodes, num_edges, false, true);
    std::vector<int> vv = G->alloc_vertex_array<int>();
    G->fill_vertex_array<int>(vv, 1);

    std::pair<uint64_t, uint64_t> *raw_edges;
    raw_edges = new std::pair<uint64_t, uint64_t>[num_edges];
    for (uint32_t i = 0; i < num_edges; i++) {
        raw_edges[i].first = edges[i].x;
        raw_edges[i].second = edges[i].y;
    }

    auto perm = get_random_permutation(num_edges);
    PRINT("=============== Load Graph BEGIN ===============");
    gettimeofday(&t_start, &tzp);
    for(uint32_t i=0; i< num_edges; i++){
        const auto &e = raw_edges[i];
        G->add_edge({e.first, e.second, 1}, true);
    }
    gettimeofday(&t_end, &tzp);
    free(edges);free(raw_edges);
    new_srcs.clear();new_dests.clear();
    PRINT("Load Graph: Nodes: " << num_nodes <<" Edges: "<<num_edges);
    print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);
    PRINT("Throughput: " << num_nodes / (float) cal_time_elapsed(&t_start, &t_end));
    PRINT("================ Load Graph END ================");
}


void init_graph(){
    num_nodes = 100000001;
    num_edges = 100000000;
    G = new Graph<uint64_t> (num_nodes, num_edges, false, true);
    std::vector<int> vv = G->alloc_vertex_array<int>();
    G->fill_vertex_array<int>(vv, 1);

    std::pair<uint64_t, uint64_t> *raw_edges;
    raw_edges = new std::pair<uint64_t, uint64_t>[num_edges];
    for (uint32_t i = 1; i < num_nodes; i++) {
        raw_edges[i].first = 0;
        raw_edges[i].second = i;
    }

    auto perm = get_random_permutation(num_edges);
    PRINT("=============== Load Graph BEGIN ===============");
    gettimeofday(&t_start, &tzp);
    for(uint32_t i=0; i< num_edges; i++){
        const auto &e = raw_edges[i];
        G->add_edge({e.first, e.second, 1}, true);
    }
    gettimeofday(&t_end, &tzp);
    free(raw_edges);
    new_srcs.clear();new_dests.clear();
    PRINT("Load Graph: Nodes: " << num_nodes <<" Edges: "<<num_edges);
    print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);
    PRINT("================ Load Graph END ================");
}



// -gname livejournal -thread -core 16 -f ../../../data/ADJgraph/livejournal.adj
int main(int argc, char** argv){
//    commandLine P(argc, argv);
//    load_graph(P);
    init_graph();
    uint64_t n = G->get_vertex_num();

    cout<<"deg: "<<G->get_outgoing_degree(0)<<endl;

    gettimeofday(&t_start, &tzp);
    auto tra = G->outgoing.get_adjlist_iter(0);
    for(auto iter=tra.first;iter!=tra.second;iter++) {
        auto edge = *iter;
        const uint64_t v = edge.nbr;
//        cout<<v<<" deg: "<<G->get_outgoing_degree(v)<<endl;
//        auto tra2 = G->outgoing.get_adjlist_iter(v);
//        int cnt = 0;
//        for(auto iter2=tra2.first;iter2!=tra2.second;iter2++) {
//            auto edge2 = *iter2;
//            const uint64_t v2 = edge2.nbr;
//            cnt ++;
//        }
    }
    gettimeofday(&t_end, &tzp);
    cout<<cal_time_elapsed(&t_start, &t_end)<<endl;

    int NN = 123456789;
    int *p = new int[NN];
    for(int i=0;i<NN;i++){
        p[i] = i;
    }


    gettimeofday(&t_start, &tzp);
    for(int i=0;i<NN;i++){
        const int pp = p[i];
    }
    gettimeofday(&t_start, &tzp);
    cout<<cal_time_elapsed(&t_start, &t_end)<<endl;

    gettimeofday(&t_start, &tzp);
    for(int i=0;i<NN;i++){
        int pp = p[i];
    }
    gettimeofday(&t_start, &tzp);
    cout<<cal_time_elapsed(&t_start, &t_end)<<endl;



}


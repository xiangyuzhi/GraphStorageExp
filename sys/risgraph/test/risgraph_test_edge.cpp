//
// Created by zxy on 5/8/22.
//
#include "risgraph_test.h"
#include "utils/io_util.h"
#include "utils/rmat_util.h"
#include "type.hpp"
#include "io.hpp"
#include "storage.hpp"

//using adjedge_type = AdjEdge<EdgeData>;
//using Storage = IndexedEdgeStorage<adjedge_type, storage::data::Vector, storage::index::DenseHashMap>;
//using adjlist_type = typename Storage::adjlist_type;

void load_graph(commandLine& P){
    auto filename = P.getOptionValue("-f", "none");
    pair_uint *edges = get_edges_from_file_adj_sym(filename.c_str(), &num_edges, &num_nodes);

    G = new Graph<void> (num_nodes, num_edges, false, true);
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
        G->add_edge({e.first, e.second}, true);
    }

    gettimeofday(&t_end, &tzp);
    free(edges);
    free(raw_edges);
    new_srcs.clear();
    new_dests.clear();
    //float size_gb = G->get_size() / (float) 1073741824;
    //PRINT("Load Graph: Nodes: " << G->get_n() <<" Edges: " << G->edges.N<< " Size: " << size_gb << " GB");
    PRINT("Load Graph: Nodes: " << num_nodes <<" Edges: "<<num_edges);
    print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);
    PRINT("Throughput: " << num_nodes / (float) cal_time_elapsed(&t_start, &t_end));
    PRINT("================ Load Graph END ================");
}




void batch_ins_del_read(){
    PRINT("=============== Batch Insert BEGIN ===============");

    std::vector<uint32_t> update_sizes = {10, 100, 1000 ,10000,100000};//,1000000,10000000
    auto r = random_aspen();
    auto update_times = std::vector<double>();
    size_t n_trials = 1;
    for (size_t us=0; us<update_sizes.size(); us++) {
        printf("GN: %u %lu\n",num_nodes,num_edges);
        double avg_insert = 0;
        double avg_delete = 0;
        double avg_read = 0;
        std::cout << "Running batch size: " << update_sizes[us] << std::endl;

        if (update_sizes[us] <= 10000000)
            n_trials = 20;
        else n_trials = 5;
        size_t updates_to_run = update_sizes[us];
        auto perm = get_random_permutation(updates_to_run);
        for (size_t ts=0; ts<n_trials; ts++) {
            uint32_t GN = num_nodes;
            new_srcs.clear();
            new_dests.clear();

            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            size_t nn = 1 << (log2_up(num_nodes) - 1);
            auto rmat = rMat<uint32_t>(nn, r.ith_rand(100+ts), a, b, c);

            std::pair<uint64_t, uint64_t> *raw_edges;
            raw_edges = new std::pair<uint64_t, uint64_t>[num_edges];
            for (uint32_t i = 0; i < updates_to_run; i++) {
                std::pair<uint32_t, uint32_t> edge = rmat(i);
                raw_edges[i].first = edge.first;
                raw_edges[i].second = edge.second;
            }
            gettimeofday(&t_start, &tzp);

            for(uint32_t i=0; i< updates_to_run; i++){
                const auto &e = raw_edges[i];
                G->add_edge({e.first, e.second}, true);
            }

            gettimeofday(&t_end, &tzp);
            avg_insert += cal_time_elapsed(&t_start, &t_end);


            gettimeofday(&t_start, &tzp);
            for(uint32_t i = 0; i < updates_to_run; i++) {
//                auto adjl = G->get_outgoing_adjlist(i);
                auto adjitr = G->get_outgoing_adjlist_range(raw_edges[i].first);
                for(auto iter = adjitr.first ; iter != adjitr.second;iter++){
                    auto edge = *iter;
                    uint64_t dst = edge.nbr;
                    if(dst == raw_edges[i].second) break;
                }
            }
            gettimeofday(&t_end, &tzp);
            avg_read += cal_time_elapsed(&t_start, &t_end);

            gettimeofday(&t_start, &tzp);
            for(uint32_t i = 0; i < updates_to_run; i++) {
                const auto &e = raw_edges[i];
                G->del_edge({e.first, e.second}, true);
            }

            gettimeofday(&t_end, &tzp);
            avg_delete +=  cal_time_elapsed(&t_start, &t_end);

            free(raw_edges);
        }
        double time_i = (double) avg_insert / n_trials;
        double insert_throughput = updates_to_run / time_i;
        printf("batch_size = %zu, average insert: %f, throughput %e\n", updates_to_run, time_i, insert_throughput);

        double time_d = (double) avg_delete / n_trials;
        double delete_throughput = updates_to_run / time_d;
        printf("batch_size = %zu, average delete: %f, throughput %e\n", updates_to_run, time_d, delete_throughput);

        double time_r = (double) avg_read / n_trials;
        double read_throughput = updates_to_run / time_r;
        printf("batch_size = %zu, average read: %f, throughput %e\n", updates_to_run, time_r, read_throughput);
    }
    PRINT("=============== Batch Insert END ===============");
}


// -src 9 -maxiters 5 -f ../../../data/slashdot.adj
// -src 9 -maxiters 5 -f ../../../data/orkut.adj
int main(int argc, char** argv) {
    srand(time(NULL));
//    printf("Num workers: %ld\n", getWorkers());
    commandLine P(argc, argv, "./graph_bm [-t testname -r rounds -f file");
    load_graph(P);

    batch_ins_del_read();

    del_G();
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}
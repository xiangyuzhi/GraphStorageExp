//
// Created by 15743 on 2022/7/27.
//

#include "risgraph_test.h"
#include "utils/io_util.h"
#include "utils/rmat_util.h"
#include "type.hpp"
#include "io.hpp"
#include "storage.hpp"
//#include "test_memory.cpp"

void load_graph(commandLine& P){
    auto filename = P.getOptionValue("-f", "none");
    pair_uint *edges = get_edges_from_file_adj_sym(filename.c_str(), &num_edges, &num_nodes);

    G = new Graph<long unsigned int> (num_nodes, num_edges, false, true);
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

double get_memory_usage() {
    FILE*file=fopen("/proc/meminfo","r");
    if(file == NULL){
        fprintf(stderr,"cannot open /proc/meminfo\n");
        return -1;
    }
    char keyword[20];
    char valuech[20];
    double mem        =0;
    double free_mem   =0;
    fscanf(file,"MemTotal: %s kB\n",keyword);
    mem=double(atol(keyword))/(1024*1024);
    fscanf(file,"MemFree: %s kB\n",valuech);
    fscanf(file,"MemAvailable: %s kB\n",valuech);
    free_mem=double(atol(valuech))/(1024*1024);
    fclose(file);
    fprintf(stderr,"Memory %.3f GB \\ %.3f GB.\n", mem, free_mem);
    return free_mem;
}

// -gname livejournal -core 16 -f ../../../data/ADJgraph/livejournal.adj -log ../../../log/aspen/mem.log
// -gname twitter -core 16 -f ../../../data/ADJgraph/twitter.adj -log ../../../log/aspen/mem.log
// -gname orkut -core 16 -f ../../../data/ADJgraph/orkut.adj -log ../../../log/aspen/mem.log
// -gname friendster -core 16 -f ../../../data/ADJgraph/friendster.adj -log ../../../log/aspen/mem.log
// -gname graph500-24 -core 16 -f ../../../data/ADJgraph/graph500-24.adj -log ../../../log/aspen/mem.log
// -gname uniform-24 -core 16 -f ../../../data/ADJgraph/uniform-24.adj -log ../../../log/aspen/mem.log
int main(int argc, char** argv) {
    //printf("Running data using %ld threads.\n", getWorkers());
    commandLine P(argc, argv);
    double before = get_memory_usage();

    load_graph(P);
    double over = get_memory_usage();

    auto log = P.getOptionValue("-log", "none");
    auto gname = P.getOptionValue("-gname", "none");

    std::ofstream alg_file(log, ios::app);
    alg_file << gname<<","<<(before - over)<< std::endl;
    alg_file.close();
    printf("%.3f \n", before - over);
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}
//
// Created by 15743 on 2022/7/27.
//

#include "terrace_test.h"
//#include "test_memory.cpp"

void load_graph(commandLine& P){
    PRINT("=============== Load Graph BEGIN ===============");
    auto filename = P.getOptionValue("-f", "none");
    pair_uint *edges = get_edges_from_file_adj_sym(filename.c_str(), &num_edges, &num_nodes);
    G = new Graph(num_nodes);
    for (uint32_t i = 0; i < num_edges; i++) {
        new_srcs.push_back(edges[i].x);
        new_dests.push_back(edges[i].y);
    }
    auto perm = get_random_permutation(num_edges);

    gettimeofday(&t_start, &tzp);
    G->add_edge_batch(new_srcs.data(), new_dests.data(), num_edges, perm);
    gettimeofday(&t_end, &tzp);
    free(edges);
    new_srcs.clear();
    new_dests.clear();
    float size_gb = G->get_size() / (float) 1073741824;
    PRINT("Load Graph: Nodes: " << G->get_num_vertices() <<" Edges: " << G->get_num_edges() << " Size: " << size_gb << " GB");
    print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);
    PRINT("Throughput: " << G->get_num_edges() / (float) cal_time_elapsed(&t_start, &t_end));
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
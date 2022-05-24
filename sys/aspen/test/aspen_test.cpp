#include "graph/api.h"
#include "trees/utils.h"

#include <cstring>
#include <cmath>
#include <iostream>
using namespace std;
using edge_seq = pair<uintV, uintV>;

template <class Graph>
void print_stats(Graph& G) {
    Graph::print_stats();
}


template <class Graph>
size_t get_sz(Graph& G, uintV src) {
    const auto& mv = G.find_vertex(src);
    return mv.value.degree();
}
template <class G>
double test_deg(G& GA, commandLine& P) {
    size_t num_sources = static_cast<size_t>(P.getOptionLongValue("-nsrc",40));
    std::cout << "Running read-vertex : with " << num_sources << " sources" << std::endl;
    size_t n = GA.num_vertices();

    timer tmr; tmr.start();
    for(size_t i =0; i < n ;i++){
        get_sz(GA, i);
    }
    tmr.stop();

//  std::cout << "avg 2-hop = " << ((total_hop*static_cast<double>(1.0)) / num_sources) << std::endl;
    return (tmr.get_total() );
}

void memory_footprint(commandLine& P) {
    // Initialize the graph.
    auto VG = initialize_treeplus_graph(P);
    treeplus_graph::print_stats();
    // Print memory statistics of the compressed graph.
    auto S = VG.acquire_version();

    S.graph.print_compression_stats();
    print_stats(S.graph);

    size_t rep_size = S.graph.size_in_bytes();
    cout << "calculated size in GB (bytes/1024**3) = " << ((rep_size*1.0)/1024/1024/1024) << endl;
}



int main(int argc, char** argv) {
    cout << "Running Aspen using " << num_workers() << " threads." << endl;
    commandLine P(argc, argv, "./memory_footprint [-f graph_file -m (mmap)]");
    memory_footprint(P);
}

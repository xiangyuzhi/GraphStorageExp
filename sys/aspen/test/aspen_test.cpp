#include "graph/api.h"
#include "trees/utils.h"

#include <cstring>
#include <cmath>
#include <iostream>
#include "utils/rmat_util.h"
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




void memory_footprint(commandLine& P) {
    // Initialize the graph.
    auto VG = initialize_treeplus_graph(P);
    treeplus_graph::print_stats();
    // Print memory statistics of the compressed graph.
    auto S = VG.acquire_version();

    //S.graph.print_compression_stats();
    //print_stats(S.graph);
    int n = S.graph.num_vertices();
    cout<<"v num : "<<n<<endl;
    uint src = 3;
    size_t deg = S.graph.find_vertex(src).value.degree();
    auto *edge = S.graph.find_vertex(src).value.get_edges(src);
    cout<<"deg: "<<deg<<endl;
    for(int i=0;i<deg;i++){
        cout<<edge[i]<<"==="<<endl;
        cout<<(bool)find_e(S.graph, src, edge[i])<<' '<<endl;
        cout<<"----"<<endl;
        for(int j = 0;j<7;j++){
            cout<<(bool) find_e(S.graph, src,j)<<endl;
        }

    }

    cout<<endl;

//    size_t nn = 1 << (pbbs::log2_up(n) - 1);
//    using pair_vertex = tuple<uintV, uintV>;
//    auto updates = pbbs::sequence<pair_vertex>(1);
//    updates[1] = make_tuple(7,1);
//    S.graph.insert_edges_batch(1, updates.begin(), false, true, nn, false);
//    S.graph.check_v();
    //size_t rep_size = S.graph.size_in_bytes();
    //cout << "calculated size in GB (bytes/1024**3) = " << ((rep_size*1.0)/1024/1024/1024) << endl;


}


// -f ../../../data/ADJgraph/LiveJournal.adj
// -f ../../../data/ADJgraph/test.adj
int main(int argc, char** argv) {
    cout << "Running Aspen using " << num_workers() << " threads." << endl;
    commandLine P(argc, argv);
    memory_footprint(P);
}

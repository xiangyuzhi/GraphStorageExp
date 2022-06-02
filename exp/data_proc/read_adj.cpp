//
// Created by zxy on 5/13/22.
//

#include "read_data.h"
#include "map"

std::vector<uint32_t> new_srcs;
std::vector<uint32_t> new_dests;
uint32_t num_nodes;
uint64_t num_edges;
struct timeval t_start, t_end;
struct timezone tzp;

template<typename Map, typename Ele>
bool madd(Map &m,Ele e){
    if(!m[e]){
        m[e] = 1;
        return true;
    }
    else m[e] ++;
    return false;
}


void load_graph(commandLine& P){
    PRINT("=============== Load Graph BEGIN ===============");
    auto filename = P.getOptionValue("-f", "none");
    pair_uint *edges = get_edges_from_file_adj_sym(filename.c_str(), &num_edges, &num_nodes);

    PRINT("================ Load Graph END ================");

    printf("Formal size: %u %lu\n",num_nodes-1, num_edges);

    for (uint32_t i = 1; i < num_edges; i++) {
        if(edges[i].x == edges[i-1].x && edges[i].y > edges[i-1].y){
            cout<<"edge false!"<<endl;
        }
    }


    // check repeated edge | own circle
//    map< pair<uint32_t, uint32_t>, uint32_t > mm;
//    bool repeated_edge = false;
//    bool own_circle = false;
//    bool directed = false;
//    bool spar = false;
//    auto *vis = new bool[num_nodes];
//    for (uint32_t i = 0; i < num_edges; i++) {
//
////        madd(vcnt,edges[i].x); madd(vcnt,edges[i].y);
//
//        if(edges[i].x == edges[i].y) own_circle = true;
//        if(!madd(mm,make_pair(edges[i].x,edges[i].y)))
//            repeated_edge = true;
//        if(mm[make_pair(edges[i].y,edges[i].x)])
//            directed = true;
//        if(edges[i].x >= num_nodes) spar = true;
//        if(edges[i].y >= num_nodes) spar = true;
//        vis[edges[i].x] = true;
//        vis[edges[i].y] = true;
////        printf("%u %u\n",edges[i].x, edges[i].y);
//    }
//    if(repeated_edge) printf(" ! Repeated Edge ! \n");
//    if(own_circle) printf("! Own Circle ! \n");
//    if(directed) printf("Directed Graph \n");
//    else printf("Maybe unDirected Graph \n");
//
//    if(spar) printf("Sparse graph\n");
//    else {
//        vector<uint32_t> not_appear;
//        for(uint32_t i=0; i < num_nodes ;i++)
//            if(!vis[i])
//                not_appear.emplace_back(i);
//
//        if(!not_appear.empty()){
//            cout<<"Not appear node size : "<<not_appear.size()<<endl;
//            if(not_appear.size()<100){
//                cout<<"Not appear node id : ";
//                for(unsigned int i : not_appear)
//                    cout<<i<<' ';
//            }
//            cout<<endl;
//        }
//    }
//    auto perm = get_random_permutation(num_edges);

}



// -t BFS -src 1 -r 4 -s -f ../../data/LiveJournal.adj
// -t BFS -src 1 -r 4 -s -f ../../data/slashdot.adj
// -t BFS -src 1 -r 4 -s -f ../../data/orkut.adj
int main(int argc, char** argv) {
    printf("Running data using %ld threads.\n", getWorkers());
    commandLine P(argc, argv, "./test_parallel [-t testname -r rounds -f file -m (mmap)]");

    load_graph(P);


    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}








//#include "graph/api.h"
//#include "trees/utils.h"
//#include "lib_extensions/sparse_table_hash.h"
//#include "pbbslib/random_shuffle.h"
//
//#include <cstring>
//#include <vector>
//#include <algorithm>
//#include <chrono>
//#include <thread>
//#include <cmath>
//#include <iostream>
//#include <fstream>
#define CILK 1
#include "aspen_test.h"
#include "utils/rmat_util.h"

using namespace std;
using edge_seq = pair<uintV, uintV>;
std::vector<uint32_t> new_srcs;
std::vector<uint32_t> new_dests;

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


void batch_ins_del_read(commandLine& P) {

    auto gname = P.getOptionValue("-gname", "none");
    std::ofstream alg_file("../../../log/aspen/edge.log",ios::app);
//    alg_file << "GRAPH" << "\t"+gname <<"\t["<<getCurrentTime0()<<']'<<std::endl;
    auto thd_num = P.getOptionLongValue("-core", 1);
    //alg_file << "Using threads :" << "\t"<<thd_num<<endl;

    string update_fname = P.getOptionValue("-update-file", "updates.dat");

    auto VG = initialize_treeplus_graph(P);

    auto S = VG.acquire_version();
    const auto& GA = S.graph;

    size_t n = GA.num_vertices();
    cout << "n = " << n << endl;
    VG.release_version(std::move(S));

    using pair_vertex = tuple<uintV, uintV>;

    auto r = pbbs::random();
    // 2. Generate the sequence of insertions and deletions

    auto update_sizes = pbbs::sequence<size_t>(7);
    update_sizes = {10,100,1000,10000,100000,1000000,10000000};//

    auto update_times = std::vector<double>();
    size_t n_trials = 3;

    size_t start = 0;
    for (size_t us=start; us<update_sizes.size(); us++) {
        double avg_insert = 0.0;
        double avg_delete = 0.0;
        double avg_read = 0.0;
        cout << "Running batch size: " << update_sizes[us] << endl;

        if (update_sizes[us] < 10000000) {
            n_trials = 20;
        }
        else {
            n_trials = 3;
        }

        for (size_t ts=0; ts<n_trials; ts++) {
            size_t updates_to_run = update_sizes[us];
            auto updates = pbbs::sequence<pair_vertex>(updates_to_run);
            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            size_t nn = 1 << (pbbs::log2_up(n) - 1);
            auto rmat = rMat<uintV>(nn, r.ith_rand(0), a, b, c);

            parallel_for(0, updates.size(), [&] (size_t i) {
                updates[i] = rmat(i);

            });

            {  // test for duplicate edges
                for (uint32_t i = 0; i < updates.size(); i++) {
                    if(find_e(S.graph,get<0>(updates[i]),get<1>(updates[i]))){
                        cout<<"Found!!!"<<endl;
                        exit(0);
                    }
                }
            }


            {
                timer st; st.start();
                VG.insert_edges_batch(update_sizes[us], updates.begin(), false, true, nn, false);
                double batch_time = st.stop();
                avg_insert += batch_time;
            }

            {
                timer st; st.start();
                for (uint32_t i =0 ; i< updates.size();i++){
                    find_e(S.graph,get<0>(updates[i]),get<1>(updates[i]));
                }
                double batch_time = st.stop();
                avg_read += batch_time;
            }


            {
                timer st; st.start();
                VG.delete_edges_batch(update_sizes[us], updates.begin(), false, true, nn, false);
                double batch_time = st.stop();
                avg_delete += batch_time;
            }

        }

        double time_i = (double) avg_insert / n_trials;
        double insert_throughput = update_sizes[us] / time_i;
        printf("batch_size = %zu, average insert: %f, throughput %e\n", update_sizes[us], time_i, insert_throughput);
//        alg_file <<"\t["<<getCurrentTime0()<<']' << "Insert"<< "\tbatch_size=" << update_sizes[us]<< "\ttime=" << time_i<< "\tthroughput=" << insert_throughput << std::endl;
        alg_file<< gname<<","<<thd_num<<",insert,"<< update_sizes[us] <<","<<insert_throughput << "\n";

        double time_r = (double) avg_read / n_trials;
        double read_throughput = update_sizes[us] / time_r;
        printf("batch_size = %zu, average read: %f, throughput %e\n", update_sizes[us], time_r, read_throughput);
//        alg_file <<"\t["<<getCurrentTime0()<<']' << "Read"<< "\tbatch_size=" << update_sizes[us]<< "\ttime=" << time_r<< "\tthroughput=" << read_throughput << std::endl;
        alg_file<< gname<<","<<thd_num<<",read,"<< update_sizes[us] <<","<<read_throughput << "\n";

        double time_d = (double) avg_delete / n_trials;
        double delete_throughput = update_sizes[us] / time_d;
        printf("batch_size = %zu, average delete: %f, throughput %e\n", update_sizes[us], time_d, delete_throughput);
//        alg_file <<"\t["<<getCurrentTime0()<<']' << "Delete"<< "\tbatch_size=" << update_sizes[us]<< "\ttime=" << time_d<< "\tthroughput=" << delete_throughput << std::endl;
        alg_file<< gname<<"," <<thd_num<<",delete,"<< update_sizes[us] <<","<<delete_throughput << "\n";

    }
}

// -gname livejournal -core 1 -f ../../../data/ADJgraph/livejournal.adj
// -gname slashdot -core 1 -f ../../../data/ADJgraph/slashdot.adj
// -gname orkut -s -core 1 -f ../../../data/ADJgraph/orkut.adj
int main(int argc, char** argv) {

    commandLine P(argc, argv);
    auto thd_num = P.getOptionLongValue("-core", 1);
    set_num_workers(thd_num);
    cout << "Running Aspen using " << num_workers() << " threads." << endl;

    batch_ins_del_read(P);
}

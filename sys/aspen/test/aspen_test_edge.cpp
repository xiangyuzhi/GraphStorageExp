#define CILK 1
//#define OPENMP 1
#include "aspen_test.h"
#include "utils/rmat_util.h"
#include "utils/util.h"

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
    auto thd_num = P.getOptionLongValue("-core", 1);
    auto log = P.getOptionValue("-log","none");
    std::ofstream log_file(log,ios::app);

    auto VG = initialize_treeplus_graph(P);
    auto S = VG.acquire_version();
    const auto& GA = S.graph;
    size_t n = GA.num_vertices();
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

        if (update_sizes[us] < 10000000)
            n_trials = 20;
        else n_trials = 5;
        size_t updates_to_run = update_sizes[us];
        for (size_t ts=0; ts<n_trials; ts++) {
            auto updates = pbbs::sequence<pair_vertex>(updates_to_run);
            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            size_t nn = 1 << (pbbs::log2_up(n) - 1);
            auto rmat = rMat<uintV>(nn, r.ith_rand(100+ts), a, b, c);

            parallel_for(0, updates.size(), [&] (size_t i) {
                updates[i] = rmat(i);
            });

            {
                timer st; st.start();
                VG.insert_edges_batch(update_sizes[us], updates.begin(), false, true, nn, false);
                double batch_time = st.stop();
                avg_insert += batch_time;
            }


//            {
//                auto read_edges = pbbs::sequence<pair_vertex>(GA.num_edges()/20);
//                parallel_for(0, read_edges.size(), [&] (size_t i) {
//                    updates[i] = rmat(i);
//                });
//                timer st; st.start();
//                for (uint32_t i =0 ; i< updates.size();i++){
//                    find_e(GA,get<0>(updates[i]),get<1>(updates[i]));
//                }
//                double batch_time = st.stop();
//                avg_read += batch_time;
//            }
//            {
//                timer st; st.start();
//                auto tmpG = VG.acquire_version();
//                for (uint32_t i =0 ; i< updates.size();i++){
//                    find_e(tmpG.graph,get<0>(updates[i]),get<1>(updates[i]));
//                }
//                VG.release_version(std::move(tmpG));
//                double batch_time = st.stop();
//                avg_read += batch_time;
//            }

            {
                timer st; st.start();
                VG.delete_edges_batch(update_sizes[us], updates.begin(), false, true, nn, false);
                double batch_time = st.stop();
                avg_delete += batch_time;
            }

        }
        double time_i = (double) avg_insert / n_trials;
        double insert_throughput = updates_to_run / time_i;
        printf("batch_size = %zu, average insert: %f, throughput %e\n", updates_to_run, time_i, insert_throughput);
        log_file<< gname<<","<<thd_num<<",e,insert,"<< update_sizes[us] <<","<<insert_throughput << "\n";

        double time_r = (double) avg_read / n_trials;
        double read_throughput = updates_to_run / time_r;
        printf("batch_size = %zu, average read: %f, throughput %e\n", updates_to_run, time_r, read_throughput);
        log_file<< gname<<","<<thd_num<<",e,read,"<< update_sizes[us] <<","<<time_r << "\n";

        double time_d = (double) avg_delete / n_trials;
        double delete_throughput = updates_to_run / time_d;
        printf("batch_size = %zu, average delete: %f, throughput %e\n", updates_to_run, time_d, delete_throughput);
        log_file<< gname<<"," <<thd_num<<",e,delete,"<< update_sizes[us] <<","<<delete_throughput << "\n";

    }
}

// -gname livejournal -core 1 -f ../../../data/ADJgraph/livejournal.adj -log ../../../log/aspen/edge.log
// -gname orkut -core 16 -f ../../../data/ADJgraph/orkut.adj -log ../../../log/aspen/edge.log
// -gname twitter -core 16 -f ../../../data/ADJgraph/twitter.adj -log ../../../log/aspen/edge.log
// -gname friendster -core 16 -f ../../../data/ADJgraph/friendster.adj -log ../../../log/aspen/edge.log
// -gname uniform-24 -core 16 -f ../../../data/ADJgraph/uniform-24.adj -log ../../../log/aspen/edge.log
// -gname graph500-24 -core 16 -f ../../../data/ADJgraph/graph500-24.adj -log ../../../log/aspen/edge.log
int main(int argc, char** argv) {

    commandLine P(argc, argv);
    auto thd_num = P.getOptionLongValue("-core", 1);
    set_num_workers(thd_num);
    cout << "Running Aspen using " << num_workers() << " threads." << endl;

    batch_ins_del_read(P);

}

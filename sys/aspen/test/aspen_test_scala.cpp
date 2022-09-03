#define CILK 1
//#define OPENMP 1
#include "aspen_test.h"
#include "utils/rmat_util.h"
#include "utils/util.h"
#include "algorithms/BFS.h"
#include "algorithms/PR.h"
#include "trees/utils.h"
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

template <class G>
double test_bfs(G& GA, commandLine& P) {
    long src = 9;
    bool thread = P.getOption("-thread");
    if(!thread) {
        uint32_t n = GA.num_vertices();
        auto get_degree = [&] (uintV i) {
            const auto& mv = GA.find_vertex(i);
            if (mv.valid) {
                return mv.value.degree();
            }
            return static_cast<size_t>(0);
        };
        size_t maxdeg = 0;
        size_t maxid = 9;
        for(int i=1;i<n;i++){
            size_t src_degree = get_degree(i);
            if(src_degree > maxdeg) {
                maxdeg = src_degree;
                maxid = i;
            }
        }
        src = maxid;
    }
    timer bfst; bfst.start();
    BFS(GA, src);
    bfst.stop();
    return bfst.get_total();
}


template <class G>
double test_k_hop(G& GA, commandLine& P, int k) {
    timer tmr; tmr.start();
//    K_HOP(GA, k);
    uint32_t n = GA.num_vertices();
    uint32_t nsrc = n/20;
    srand(n);

    parallel_for(0, nsrc, [&] (size_t i) {
        auto src = rand()%n;
        auto map_f = [&] (uintV ngh_id, size_t ind) {
            if(k==2){
                auto map_f2 = [&] (uintV nnid, size_t ind) {};
                const auto& v2 = GA.find_vertex(ngh_id).value;
                v2.map_elms(ngh_id, map_f2);
            }
        };
        const auto& v = GA.find_vertex(src).value;
        v.map_elms(src, map_f);
    });
    tmr.stop();
    return (tmr.get_total());
}


template <class G>
double test_pr(G& GA, commandLine& P) {
    long maxiters = P.getOptionLongValue("-maxiters",10);
    timer tmr; tmr.start();
    PR<double,G>(GA,maxiters);
    tmr.stop();
    return (tmr.get_total());
}

template <class G>
double test_read(G& GA, commandLine& P) {
    uint64_t n = GA.num_vertices();
    auto r = pbbs::random();
    using pair_vertex = tuple<uintV, uintV>;
    auto updates = pbbs::sequence<pair_vertex>(GA.num_edges()/20);
    double a = 0.5;
    double b = 0.1;
    double c = 0.1;
    size_t nn = 1 << (pbbs::log2_up(n) - 1);
    auto rmat = rMat<uintV>(nn, r.ith_rand(100), a, b, c);
    parallel_for(0, updates.size(), [&] (size_t i) {
        updates[i] = rmat(i);
    });
    timer tmr; tmr.start();
    parallel_for(0, updates.size(), [&] (size_t i){
        find_e(GA,get<0>(updates[i]),get<1>(updates[i]));
    });
    tmr.stop();
    return (tmr.get_total());
}


template <class Graph>
double execute(Graph& G, commandLine& P, string testname) {
    if (testname == "BFS") {
        return test_bfs(G, P);
    } else if (testname == "PR") {
        return test_pr(G, P);
    } else if (testname == "1-HOP") {
        return test_k_hop(G, P, 1);
    }else if (testname == "2-HOP") {
        return test_k_hop(G, P, 2);
    } else if (testname == "Read") {
        return test_read(G, P);
    } else {
        std::cout << "Unknown test: " << testname << ". Quitting." << std::endl;
        exit(0);
    }
}

template<typename Graph>
void run_algorithm(commandLine& P, int thd_num, Graph &VG, string gname) {
    cout<<"=============== Run Algorithm BEGIN ==============="<<endl;

    size_t rounds = P.getOptionLongValue("-rounds", 5);
    auto log = P.getOptionValue("-log", "none");
    std::ofstream alg_file(log, ios::app);

    std::vector<std::string> test_ids;
    test_ids = {"BFS","1-HOP","2-HOP","Read"};//"PR",

    for (auto test_id : test_ids) {
        std::vector<double> total_time;
        for (size_t i = 0; i < rounds; i++) {
            auto S = VG.acquire_version();
            double tm = execute(S.graph, P, test_id);

            std::cout << "\ttest=" << test_id << "\ttime=" << tm << "\titeration=" << i << std::endl;
            total_time.emplace_back(tm);
            VG.release_version(std::move(S));
        }
        double avg_time = cal_time(total_time);
        std::cout << "\ttest=" << test_id<< "\ttime=" << avg_time << "\tgraph=" << gname << std::endl;
        alg_file << gname<<","<< thd_num<<","<<test_id<<","<< avg_time << std::endl;
    }
    alg_file.close();
    cout<<"=============== Run Algorithm END ==============="<<endl;
}


template<typename Graph>
void batch_ins_del_read(commandLine& P, int thd_num, Graph &VG, string gname) {
    auto log = P.getOptionValue("-log","none");
    std::ofstream log_file(log,ios::app);

    auto S = VG.acquire_version();
    const auto& GA = S.graph;
    size_t n = GA.num_vertices();
    VG.release_version(std::move(S));
    using pair_vertex = tuple<uintV, uintV>;
    auto r = pbbs::random();

    // 2. Generate the sequence of insertions and deletions
    auto update_sizes = pbbs::sequence<size_t>(1);
    update_sizes = {100000};//
    auto update_times = std::vector<double>();
    size_t n_trials = 3;

    size_t start = 0;
    for (size_t us=start; us<update_sizes.size(); us++) {
        double avg_insert = 0.0;
        double avg_delete = 0.0;
        double avg_read = 0.0;
        cout << "Running batch size: " << update_sizes[us] << endl;

        if (update_sizes[us] < 10000000)
            n_trials = 1;
        else n_trials = 1;
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

        double time_d = (double) avg_delete / n_trials;
        double delete_throughput = updates_to_run / time_d;
        printf("batch_size = %zu, average delete: %f, throughput %e\n", updates_to_run, time_d, delete_throughput);
        log_file<< gname<<"," <<thd_num<<",e,delete,"<< update_sizes[us] <<","<<delete_throughput << "\n";
    }
}



int main(int argc, char** argv) {

    commandLine P(argc, argv);
    bool thread = P.getOption("-thread");
    if(thread){
        {
            auto gname = P.getOptionValue("-gname", "none");
            std::vector<uint32_t> threads = {1,4,8,12,16};
            auto VG = initialize_treeplus_graph(P);
            for(auto thd_num : threads){
                set_num_workers(thd_num);
                cout << "Running Aspen using " << num_workers() << " threads." << endl;
                run_algorithm(P, thd_num, VG, gname);
            }

            for(auto thd_num: threads){
                set_num_workers(thd_num);
                cout << "Running Aspen using " << num_workers() << " threads." << endl;
                batch_ins_del_read(P, thd_num, VG, gname);
            }
        }
    }

    else {
        set_num_workers(16);
        auto insert_f = [&](uint32_t deg, uint32_t logv) {
            cout << "v: " << logv << " e: " << deg << endl;
            auto VG = empty_treeplus_graph();
            auto r = pbbs::random();
            using pair_vertex = tuple<uintV, uintV>;
            uint32_t v = (1L << logv);
            uint32_t e = (1L << logv) * deg;
            num_edges = e;
            auto updates = pbbs::sequence<pair_vertex>(e);
            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            size_t nn = 1 << (pbbs::log2_up((1L << logv)) - 1);
            auto rmat = rMat<uintV>(v, r.ith_rand(100), a, b, c);
            parallel_for(0, updates.size(), [&](size_t i) {
                updates[i] = rmat(i);
            });
            VG.insert_edges_batch(e, updates.begin(), false, true, v, false);

            string gname = "graph_" + to_string(logv) + "_" + to_string(deg);
            run_algorithm(P, 16, VG, gname);
            batch_ins_del_read(P, 16, VG, gname);
        };

        {
            auto v = P.getOptionIntValue("-v", -1);
            auto e = P.getOptionIntValue("-e", -1);
            insert_f(e, v);
        }
    }

}

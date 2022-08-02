//#define OPENMP 1
#define CILK 1

#include "graph/api.h"
#include "algorithms/BFS.h"
#include "algorithms/BC.h"
#include "algorithms/LDD.h"
#include "algorithms/k_hop.h"
#include "algorithms/TC.h"
#include "algorithms/mutual_friends.h"
#include "algorithms/MIS.h"
#include "algorithms/Nibble.h"
#include "algorithms/PR.h"
#include "algorithms/CC.h"
#include "algorithms/LP.h"
#include "trees/utils.h"
#include "utils/rmat_util.h"

#include <cstring>
#include <chrono>
#include <thread>
#include <cmath>
#include <random>


double cal_time(std::vector<double> timelist){
    if(timelist.size() == 1) return timelist[0];
    sort(timelist.begin(),timelist.end());
    double st = 0.0;
    for(uint32_t i = 1 ;i < timelist.size()-1;i++)
        st += timelist[i];
    return st/double(timelist.size()-2);
}


template <class G>
double test_bfs(G& GA, commandLine& P) {
    long src = P.getOptionLongValue("-src",9);
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
        const auto& v = GA.find_vertex(src).value;
        auto map_f = [&] (uintV ngh_id, size_t ind) {
            if(k==2){
                auto map_f2 = [&] (uintV nnid, size_t ind) {};
                const auto& v2 = GA.find_vertex(ngh_id).value;
                v2.map_elms(ngh_id, map_f2);
            }
            //GA.find_vertex(ngh_id).value.degree();
        };
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
double test_cc(G& GA, commandLine& P) {

    timer tmr; tmr.start();
    CC<G>(GA);
    tmr.stop();
    return (tmr.get_total());
}

template <class G>
double test_lp(G& GA, commandLine& P) {

    long maxiters = P.getOptionLongValue("-maxiters",10);

    timer tmr; tmr.start();
    LP(GA,maxiters);
    tmr.stop();
    return (tmr.get_total());
}


template <class G>
double test_tc(G& GA, commandLine& P) {
    timer tmr; tmr.start();
    auto count = TC(GA);
    tmr.stop();
    return (tmr.get_total());
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

template <class G>
double test_read(G& GA, commandLine& P) {
    auto r = pbbs::random();
    uint64_t n = GA.num_vertices();
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
    } else if (testname == "CC") {
        return test_cc(G, P);
    } else if (testname == "1-HOP") {
        return test_k_hop(G, P, 1);
    }else if (testname == "2-HOP") {
        return test_k_hop(G, P, 2);
    } else if (testname == "TC") {
        return test_tc(G, P);
    } else if (testname == "LP") {
        return test_lp(G, P);
    } else if (testname == "Read") {
        return test_read(G, P);
    } else {
        std::cout << "Unknown test: " << testname << ". Quitting." << std::endl;
        exit(0);
    }
}

void run_algorithm(commandLine& P) {
    cout<<"=============== Run Algorithm BEGIN ==============="<<endl;
    auto VG = initialize_treeplus_graph(P);
    // Run the algorithm on it

    size_t rounds = P.getOptionLongValue("-rounds", 4);
    auto gname = P.getOptionValue("-gname", "none");
    auto thd_num = P.getOptionLongValue("-core", 1);
    auto log = P.getOptionValue("-log", "none");
    std::ofstream alg_file(log, ios::app);

    std::vector<std::string> test_ids;
//    test_ids = {"1-HOP","2-HOP","BFS","SSSP","PR","CC","LP","Read","TC"};
    test_ids = {"1-HOP","2-HOP","Read"};

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


// -gname livejournal -core 16 -f ../../../data/ADJgraph/livejournal.adj -log ../../../log/aspen/alg.log
// -gname friendster -core 16 -f ../../../data/ADJgraph/friendster.adj -log ../../../log/aspen/alg.log
int main(int argc, char** argv) {

    commandLine P(argc, argv );
    auto thd_num = P.getOptionLongValue("-core", 1);
    set_num_workers(thd_num);
    printf("Running Aspen using %ld threads.\n", thd_num );

    run_algorithm(P);
}

//#define OPENMP 1
#define CILK 1

#include "graph/api.h"
#include "algorithms/BFS.h"
#include "algorithms/BC.h"
#include "algorithms/LDD.h"
//#include "algorithms/k-Hop.h"
#include "algorithms/k_hop.h"
#include "algorithms/TC.h"
#include "algorithms/mutual_friends.h"
#include "algorithms/MIS.h"
#include "algorithms/Nibble.h"
#include "algorithms/PR.h"
#include "algorithms/CC.h"
#include "algorithms/LP.h"
#include "trees/utils.h"


#include <cstring>
#include <chrono>
#include <thread>
#include <cmath>
#include <random>

static std::string getCurrentTime0() {
    std::time_t result = std::time(nullptr);
    std::string ret;
    ret.resize(64);
    int wsize = sprintf((char *)&ret[0], "%s", std::ctime(&result));
    ret.resize(wsize-1);
    return ret;
}

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
    K_HOP(GA, k);
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
    test_ids = {"BFS","PR","LP","CC","TC","1-HOP","2-HOP"};//

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

//
// Created by 15743 on 2022/6/10.
//

#ifndef EXP_ALG_H
#define EXP_ALG_H

#include "algorithms/BFS.h"
#include "algorithms/k-hop.h"
#include "algorithms/TC.h"
#include "algorithms/PR.h"
#include "algorithms/SSSP.h"
#include "algorithms/CC.h"
#include "algorithms/LP.h"

#include "omp.h"
#include <cstring>
#include <chrono>
#include <thread>
#include <cmath>
#include <random>
#include "utils/rmat_util.h"
#include "utils/util.h"


struct timeval t_start, t_end;
struct timezone tzp;

template <class G>
double test_bfs(G& GA, commandLine& P) {
    long bfs_src = P.getOptionLongValue("-src",9);
    std::cout << "Running BFS from source = " << bfs_src << std::endl;
    gettimeofday(&t_start, &tzp);
    BFS(GA, bfs_src);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}


template <class G>
double test_pr(G& GA, commandLine& P) {
    long maxiters = P.getOptionLongValue("-maxiters",10);
    gettimeofday(&t_start, &tzp);
    PR(GA, maxiters);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

template <class G>
double test_sssp(G& GA, commandLine& P){
    long sssp_src = P.getOptionLongValue("-src",9);
    gettimeofday(&t_start, &tzp);
    SSSP(GA, sssp_src);
    gettimeofday(&t_end, &tzp);

    return cal_time_elapsed(&t_start, &t_end);
}


template <class G>
double test_cc(G& GA, commandLine& P) {
    gettimeofday(&t_start, &tzp);
    CC(GA);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

template <class G>
double test_k_hop(G& GA, commandLine& P, int k) {
    gettimeofday(&t_start, &tzp);
    K_HOP(GA, k);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}


template <class G>
double test_lp(G& GA, commandLine& P) {
    long maxiters = P.getOptionLongValue("-maxiters",10);
    gettimeofday(&t_start, &tzp);
    LP(GA, maxiters);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}



template <class G>
double test_tc(G& GA, commandLine& P) {
    gettimeofday(&t_start, &tzp);
    TC(GA);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

template <class Graph>
double execute(Graph& G, commandLine& P, string testname) {
    if (testname == "BFS") {
        return test_bfs(G, P);
    } else if (testname == "PR") {
        return test_pr(G, P);
    } else if (testname == "SSSP") {
        return test_sssp(G, P);
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

template<typename Graph>
void run_algorithm(commandLine& P, Graph& G) {
    PRINT("=============== Run Algorithm BEGIN ===============");

    size_t rounds = P.getOptionLongValue("-rounds", 4);
    std::vector<std::string> test_ids;
    test_ids = {"BFS","PR","SSSP","CC","1-HOP","2-HOP","LP","TC"};

    auto gname = P.getOptionValue("-gname", "none");
    auto log = P.getOptionValue("-log", "none");
    std::ofstream alg_file(log, ios::app);
    auto thd_num = P.getOptionLongValue("-core", 1);

    for (auto test_id : test_ids) {
        std::vector<double> total_time;
        for (size_t i=0; i<rounds; i++) {
            double tm = execute(G, P, test_id);
            std::cout << "\ttest=" << test_id << "\ttime=" << tm << "\titeration=" << i << std::endl;
            total_time.emplace_back(tm);
        }
        double avg_time = cal_time(total_time);
        std::cout << "\ttest=" << test_id<< "\ttime=" << avg_time << "\tgraph=" << gname << std::endl;
        alg_file << gname<<","<< thd_num<<","<<test_id<<","<< avg_time << std::endl;
    }
    alg_file.close();
    PRINT("=============== Run Algorithm END ===============");
}

#endif //EXP_ALG_H

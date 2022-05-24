//
// Created by zxy on 5/9/22.
//

#include "stinger_test.h"
#include "BFS.h"
#include "PR.h"
#include "SSSP.h"
#include "CC.h"
#include "k-hop.h"
#include "LP.h"
#include "TC.h"


double test_bfs(commandLine& P) {
    long bfs_src = P.getOptionLongValue("-src",-1);
    if (bfs_src == -1) {
        std::cout << "Please specify a source vertex to run the BFS from using -src" << std::endl;
        exit(0);
    }
    std::cout << "Running BFS from source = " << bfs_src << std::endl;

    gettimeofday(&t_start, &tzp);
    BFS(G, bfs_src);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}


double test_pr(commandLine& P) {
    long maxiters = P.getOptionLongValue("-maxiters",10);
    gettimeofday(&t_start, &tzp);
    PR<double, stinger>(G, maxiters);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double test_sssp(commandLine& P){
    long sssp_src = P.getOptionLongValue("-src",-1);
    if (sssp_src == -1) {
        std::cout << "Please specify a source vertex to run the SSSP from" << std::endl;
        exit(0);
    }
    gettimeofday(&t_start, &tzp);
    SSSP(G, sssp_src);
    gettimeofday(&t_end, &tzp);

    return cal_time_elapsed(&t_start, &t_end);
}

double test_cc(commandLine& P) {
    gettimeofday(&t_start, &tzp);
    CC(G);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double test_k_hop(commandLine& P, int k) {
    gettimeofday(&t_start, &tzp);
    K_HOP(G, k);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double test_lp(commandLine& P) {

    long maxiters = P.getOptionLongValue("-maxiters",10);
    gettimeofday(&t_start, &tzp);
    LP(G, maxiters);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double test_tc(commandLine& P) {

    gettimeofday(&t_start, &tzp);
    auto count = TC(G);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double execute(commandLine& P, string testname) {
    if (testname == "BFS") {
        return test_bfs(P);
    } else if (testname == "PR") {
        return test_pr(P);
    } else if (testname == "SSSP") {
        return test_sssp(P);
    } else if (testname == "CC") {
        return test_cc(P);
    } else if (testname == "1-HOP") {
        return test_k_hop(P, 1);
    }else if (testname == "2-HOP") {
        return test_k_hop(P, 2);
    } else if (testname == "LP") {
        return test_lp(P);
    } else if (testname == "TC") {
        return test_tc(P);
    } else {
        std::cout << "Unknown test: " << testname << ". Quitting." << std::endl;
        exit(0);
    }
}

void run_algorithm(commandLine& P) {
    PRINT("=============== Run Algorithm BEGIN ===============");

    auto filename = P.getOptionValue("-f", "none");
    size_t rounds = P.getOptionLongValue("-rounds", 4);
    std::vector<std::string> test_ids;
    // if testname is TC, include it, otherwise exclude it
    test_ids = {"BFS","PR","SSSP","CC","1-HOP","2-HOP","LP","TC"};

    for (auto test_id : test_ids) {
        double total_time = 0.0;
        for (size_t i=0; i<rounds; i++) {
            double tm = execute(P, test_id);

            std::cout << "\ttest=" << test_id
                      << "\ttime=" << tm
                      << "\titeration=" << i << std::endl;
            total_time += tm;
        }
        std::cout << "AVG"
                  << "\ttest=" << test_id
                  << "\ttime=" << (total_time / rounds)
                  << "\tgraph=" << filename << std::endl;
    }

    PRINT("=============== Run Algorithm END ===============");
}


// -t BFS -src 1 -r 4 -s -f ../../../data/LiveJournal.adj
// -t BFS -src 1 -r 4 -s -f ../../../data/slashdot.adj
int main(int argc, char** argv) {
    printf("Running LiveGraph using %ld threads.\n", getWorkers());
    commandLine P(argc, argv, "./test_parallel [-t testname -r rounds -f file -m (mmap)]");

    load_graph(P);
    run_algorithm(P);
    del_G();

    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}


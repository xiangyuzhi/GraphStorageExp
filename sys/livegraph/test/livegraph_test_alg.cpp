//
// Created by zxy on 5/9/22.
//
#define OPENMP 1
#include "livegraph_test.h"
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
    PR<double>(G, maxiters);
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
    test_ids = {"1-HOP","2-HOP"};//,"BFS","PR","SSSP","CC","LP","TC"

    auto gname = P.getOptionValue("-gname", "none");
    std::ofstream alg_file("../../../log/livegraph/alg.log",ios::app);
    alg_file << "GRAPH" << "\t"+gname <<"\t["<<getCurrentTime0()<<']'<<std::endl;
    auto thd_num = P.getOptionLongValue("-core", 1);
    alg_file << "Using threads :" << "\t"<<thd_num<<endl;

    for (auto test_id : test_ids) {
        std::vector<double> total_time;
        for (size_t i=0; i<rounds; i++) {
            double tm = execute(P, test_id);

            std::cout << "\ttest=" << test_id << "\ttime=" << tm << "\titeration=" << i << std::endl;
            total_time.emplace_back(tm);
        }
        double avg_time = cal_time(total_time);
        std::cout <<"["<<getCurrentTime0()<<']'<< "AVG"<< "\ttest=" << test_id<< "\ttime=" << avg_time << "\tgraph=" << filename << std::endl;
        alg_file <<"\t["<<getCurrentTime0()<<']' << "AVG"<< "\ttest=" << test_id<< "\ttime=" << avg_time << std::endl;
    }
    alg_file.close();
    PRINT("=============== Run Algorithm END ===============");
}


// -src 9 -s -gname LiveJournal -core 1 -f ../../../data/ADJgraph/LiveJournal.adj
// -t BFS -src 1 -r 4 -s -f ../../../data/slashdot.adj
int main(int argc, char** argv) {
    commandLine P(argc, argv );
    auto thd_num = P.getOptionLongValue("-core", 1);
    omp_set_num_threads(thd_num);
    printf("Running LiveGraph using %ld threads.\n", thd_num );

    load_graph(P);
    run_algorithm(P);
    del_G();

    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}


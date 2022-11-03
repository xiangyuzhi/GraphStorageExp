//
// Created by zxy on 5/9/22.
//
//#define CILK 1
#define  OPENMP 1
#include "graph1_test.h"
#include "BFS.h"
#include "PR.h"
#include "SSSP.h"
#include "CC.h"
#include "k-hop.h"
#include "LP.h"
#include "TC.h"
#include "util/parallel.h"

double test_bfs(commandLine& P) {
    long bfs_src = P.getOptionLongValue("-src",9);
    std::cout << "Running BFS from source = " << bfs_src << std::endl;

    gettimeofday(&t_start, &tzp);
    auto* view = create_static_view(get_graphone_graph(), SIMPLE_MASK | PRIVATE_MASK);
    BFS(view, bfs_src);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}


double test_pr(commandLine& P) {
    long maxiters = P.getOptionLongValue("-maxiters",10);
    gettimeofday(&t_start, &tzp);
    auto* view = create_static_view(get_graphone_graph(), SIMPLE_MASK | PRIVATE_MASK);
    PR<double>(view, maxiters);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double test_sssp(commandLine& P){
    long sssp_src = P.getOptionLongValue("-src",9);
    gettimeofday(&t_start, &tzp);
    auto* view = create_static_view(get_graphone_graph(), SIMPLE_MASK | PRIVATE_MASK);
    SSSP(view, sssp_src);
    gettimeofday(&t_end, &tzp);

    return cal_time_elapsed(&t_start, &t_end);
}

double test_cc(commandLine& P) {
    gettimeofday(&t_start, &tzp);
    auto* view = create_static_view(get_graphone_graph(), SIMPLE_MASK | PRIVATE_MASK);
    CC(view);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double test_k_hop(commandLine& P, int k) {
    gettimeofday(&t_start, &tzp);
    auto* view = create_static_view(get_graphone_graph(), SIMPLE_MASK | PRIVATE_MASK);
    //K_HOP(view, k);
    if(k==1){
        uint32_t n = view->v_count;
        uint32_t nsrc = n/20;
        srand(n);
        parallel_for(int i=0;i<nsrc;i++){
            auto src = rand()%n;
            uint64_t degree = view->get_degree_out(src);
            lite_edge_t* neighbours = nullptr;
            neighbours = (lite_edge_t*) realloc(neighbours, sizeof(neighbours[0]) * degree);
            degree = view->get_nebrs_out(src, neighbours);
            for(uint32_t local_idx = 0; local_idx < degree; local_idx++){
                uint64_t v = get_sid(neighbours[local_idx]);
                uint32_t w = neighbours[local_idx].second.value32b;
            }
            free(neighbours); neighbours = nullptr;
        }
    }
    else {
        uint32_t n = view->v_count;
        uint32_t nsrc = n/20;
        srand(n);
        parallel_for(int i=0;i<nsrc;i++){
            auto src = rand()%n;
            uint64_t degree = view->get_degree_out(src);
            lite_edge_t* neighbours = nullptr;
            neighbours = (lite_edge_t*) realloc(neighbours, sizeof(neighbours[0]) * degree);
            degree = view->get_nebrs_out(src, neighbours);
            for(uint32_t id1 = 0; id1 < degree; id1++){
                uint64_t v = get_sid(neighbours[id1]);
                uint32_t w = neighbours[id1].second.value32b;
                uint64_t deg2 = view->get_degree_out(v);
                lite_edge_t* ngh2 = nullptr;
                ngh2 = (lite_edge_t*) realloc(ngh2, sizeof(ngh2[0]) * deg2);
                deg2 = view->get_nebrs_out(v, ngh2);
                for(uint32_t id2 = 0; id2 < deg2; id2++) {
                    uint64_t v = get_sid(ngh2[id2]);
                }
                free(ngh2); ngh2 = nullptr;
            }
            free(neighbours); neighbours = nullptr;
        }
    }

    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}


double test_lp(commandLine& P) {

    long maxiters = P.getOptionLongValue("-maxiters",10);
    gettimeofday(&t_start, &tzp);
    auto* view = create_static_view(get_graphone_graph(), SIMPLE_MASK | PRIVATE_MASK);
    LP(view,maxiters);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}


double test_tc(commandLine& P) {

    gettimeofday(&t_start, &tzp);
    auto* view = create_static_view(get_graphone_graph(), SIMPLE_MASK | PRIVATE_MASK);
    auto count = TC(view);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}


double test_read(commandLine& P) {
    auto* view = create_static_view(get_graphone_graph(), SIMPLE_MASK | PRIVATE_MASK);
    auto r = random_aspen();
    size_t n = view->v_count;
    double a = 0.5;
    double b = 0.1;
    double c = 0.1;
    size_t nn = 1 << (log2_up(n) - 1);
    auto rmat = rMat<uint32_t>(nn, r.ith_rand(100), a, b, c);
    new_srcs.clear();new_dests.clear();
    uint32_t updates = num_edges/20;
    for( uint32_t i = 0; i < updates; i++) {
        std::pair<uint32_t, uint32_t> edge = rmat(i);
        new_srcs.push_back(edge.first);
        new_dests.push_back(edge.second);
    }
    gettimeofday(&t_start, &tzp);
    parallel_for (uint32_t i =0 ; i< updates;i++){
        uint64_t degree = view->get_degree_out(new_srcs[i]);

        lite_edge_t* neighbours = nullptr;
        neighbours = (lite_edge_t*) realloc(neighbours, sizeof(neighbours[0]) * degree);
        degree = view->get_nebrs_out(new_srcs[i], neighbours);

        for(uint32_t local_idx = 0; local_idx < degree; local_idx++){
            uint64_t v = get_sid(neighbours[local_idx]);
            if(v == new_dests[i]) break;
        }
        free(neighbours); neighbours = nullptr;
    }

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
    } else if (testname == "Read") {
        return test_read(P);
    } else {
        std::cout << "Unknown test: " << testname << ". Quitting." << std::endl;
        exit(0);
    }
}

void run_algorithm(commandLine& P) {
    PRINT("=============== Run Algorithm BEGIN ===============");

    // Run the algorithm on it
    size_t rounds = P.getOptionLongValue("-rounds", 5);
    auto gname = P.getOptionValue("-gname", "none");
    auto thd_num = P.getOptionLongValue("-core", 1);
    auto log = P.getOptionValue("-log", "none");
    std::ofstream alg_file(log, ios::app);

    std::vector<std::string> test_ids;
    test_ids = {"Read","1-HOP","2-HOP","BFS","SSSP","PR","CC","LP"};
//    test_ids = {"LP","TC"};

    for (auto test_id : test_ids) {
        std::vector<double> total_time;
        for (size_t i = 0; i < rounds; i++) {
            double tm = execute(P, test_id);

            std::cout << "\ttest=" << test_id << "\ttime=" << tm << "\titeration=" << i << std::endl;
            total_time.emplace_back(tm);
        }
        double avg_time = cal_time(total_time);
        std::cout << "\ttest=" << test_id<< "\ttime=" << avg_time << "\tgraph=" << gname << std::endl;
        alg_file << gname<<","<< thd_num<<","<<test_id<<","<< avg_time << std::endl;
    }
    PRINT("=============== Run Algorithm END ===============");
}


// -gname livejournal -core 16 -f ../../../data/ADJgraph/livejournal.adj -log ../../../log/graphone/alg.log
int main(int argc, char** argv) {
    commandLine P(argc, argv );
    auto thd_num = P.getOptionLongValue("-core", 1);
    set_num_workers(thd_num);
    printf("Running GraphOne using %ld threads.\n", thd_num );

    load_graph(P);
    run_algorithm(P);
    del_g();

    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}


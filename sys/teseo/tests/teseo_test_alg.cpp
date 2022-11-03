#define OPENMP 1
#include "teseo_test.h"
#include "BFS.h"
#include "PR.h"
#include "SSSP.h"
#include "CC.h"
#include "k-hop.h"
#include "LP.h"
#include "TC.h"

void teseo_pagerank(OpenMP& openmp, uint64_t num_iterations, double damping_factor = 0.8) {
    // init
    const uint64_t num_vertices = openmp.transaction().num_vertices();
    const double init_score = 1.0 / num_vertices;
    const double base_score = (1.0 - damping_factor) / num_vertices;
    auto scores =  new double[num_vertices](); // avoid memory leaks
#pragma omp parallel for
    for(uint64_t v = 0; v < num_vertices; v++){
        scores[v] = init_score;
    }
    vector<double> outgoing_contrib(num_vertices, 0.0);

    // pagerank iterations
    for(uint64_t iteration = 0; iteration < num_iterations; iteration++){
        double dangling_sum = 0.0;

        // for each node, precompute its contribution to all of its outgoing neighbours and, if it's a sink,
        // add its rank to the `dangling sum' (to be added to all nodes).
#pragma omp parallel for reduction(+:dangling_sum) firstprivate(openmp)
        for(uint64_t v = 0; v < num_vertices; v++){
            uint64_t out_degree = openmp.transaction().degree(v, /* logical */ false);
            if(out_degree == 0){ // this is a sink
                dangling_sum += scores[v];
            } else {
                outgoing_contrib[v] = scores[v] / out_degree;
            }
        }

        dangling_sum /= num_vertices;

        // compute the new score for each node in the graph
#pragma omp parallel for schedule(dynamic, 512) firstprivate(openmp)
        for(uint64_t v = 0; v < num_vertices; v++){

            double incoming_total = 0;
            openmp.iterator().edges(v, /* logical ? */ false, [&incoming_total, &outgoing_contrib](uint64_t destination){
                incoming_total += outgoing_contrib[destination];
            });

            // update the score
            scores[v] = base_score + damping_factor * (incoming_total + dangling_sum);
        }
    }
}


double test_bfs(commandLine& P) {
    uint32_t bfs_src = P.getOptionLongValue("-src",9);
    std::cout << "Running BFS from source = " << bfs_src << std::endl;
    OpenMP &omp = *OMP;
    gettimeofday(&t_start, &tzp);
    BFS(omp, bfs_src);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}


double test_pr(commandLine& P) {
    long maxiters = P.getOptionLongValue("-maxiters",10);
    OpenMP &omp = *OMP;
    gettimeofday(&t_start, &tzp);
//    PR<double>(omp, maxiters);
    teseo_pagerank(omp, maxiters);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double test_sssp(commandLine& P){
    long sssp_src = P.getOptionLongValue("-src",9);
    OpenMP &omp = *OMP;
    gettimeofday(&t_start, &tzp);
    SSSP(omp, sssp_src);
    gettimeofday(&t_end, &tzp);

    return cal_time_elapsed(&t_start, &t_end);
}

double test_cc(commandLine& P) {
    OpenMP &omp = *OMP;
    gettimeofday(&t_start, &tzp);
    CC(omp);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double test_k_hop(commandLine& P, int k) {
    OpenMP &ompp = *OMP;
    gettimeofday(&t_start, &tzp);
    //K_HOP(omp, k);
    uint64_t n = ompp.transaction().num_vertices();
    int nsrc = n/20;
    srand(n);
    #pragma omp parallel for schedule(dynamic, 1) firstprivate(ompp)
    for(int i = 0; i<nsrc;i++){
        uint64_t src = rand()%n;
        ompp.iterator().edges(src, /* logical ? */ false, [&](uint64_t v){
            if(k==2){
                ompp.iterator().edges((uint64_t)v,/* logical ? */ false, [&](uint64_t v2){});
            }
        });
    }
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double test_lp(commandLine& P) {

    long maxiters = P.getOptionLongValue("-maxiters",10);
    OpenMP &omp = *OMP;
    gettimeofday(&t_start, &tzp);
    LP(omp, maxiters);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double test_tc(commandLine& P) {
    OpenMP &omp = *OMP;
    gettimeofday(&t_start, &tzp);
    auto count = TC(omp);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}


double test_read(commandLine& P) {
    OpenMP &omp = *OMP;
    auto r = random_aspen();
    uint64_t n = omp.transaction().num_vertices();
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
    #pragma omp parallel for schedule(dynamic, 512) firstprivate(omp)
    for(uint32_t i = 0; i < updates; i++) {
        if(omp.transaction().has_edge(new_srcs[i], new_dests[i]))
            omp.transaction().get_weight(new_srcs[i], new_dests[i]);
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

    std::vector<std::string> test_ids;
    test_ids = {"Read","1-HOP","2-HOP","BFS","PR","CC","LP","TC","SSSP"};//
//    test_ids = {"BFS","SSSP","PR","CC","LP","TC"};
//    test_ids = {"1-HOP","2-HOP","Read"};
//    test_ids = {"PR"};
//    test_ids = {"BFS"};

    size_t rounds = P.getOptionLongValue("-rounds", 5);
    auto gname = P.getOptionValue("-gname", "none");
    auto thd_num = P.getOptionLongValue("-core", 1);
    auto log = P.getOptionValue("-log", "none");
    std::ofstream alg_file(log, ios::app);

    for (auto test_id : test_ids) {
        std::vector<double> total_time;
        for (size_t i=0; i<rounds; i++) {
            double tm = execute(P, test_id);
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


// -gname livejournal -core 16 -f ../../../data/ADJgraph/livejournal.adj -log ../../../log/teseo/alg.log
// -gname slashdot -core 16 -f ../../../data/ADJgraph/slashdot.adj -log ../../../log/teseo/alg.log
// -gname graph500-24 -core 16 -f ../../../data/ADJgraph/graph500-24.adj -log ../../../log/teseo/alg.log
// -gname orkut -core 16 -f ../../../data/ADJgraph/orkut.adj -log ../../../log/teseo/alg.log
int main(int argc, char** argv) {

    commandLine P(argc, argv );
    auto thd_num = P.getOptionLongValue("-core", 1);
    omp_set_num_threads(thd_num);
    printf("Running Teseo using %ld threads.\n", thd_num );

    load_graph(P);
    run_algorithm(P);
    del_G();

    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}


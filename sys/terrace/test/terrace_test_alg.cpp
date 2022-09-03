//
// Created by zxy on 4/24/22.
//

#define ENABLE_LOCK 1
#define WEIGHTED 0
#define VERIFY 0
#define CILK 1
//#define OPENMP 1

#include "terrace_test.h"
#include "BellmanFordUnweighted.h"
#include "BFS.h"
#include "Pagerank.h"
#include "Components.h"
#include "BC.h"
#include "TC.h"
#include "k_hop.h"
#include "LP.h"

#include <sys/stat.h>
#include <sys/types.h>


void load_graph(commandLine& P){
    PRINT("=============== Load Graph BEGIN ===============");
    auto filename = P.getOptionValue("-f", "none");
    pair_uint *edges = get_edges_from_file_adj_sym(filename.c_str(), &num_edges, &num_nodes);
    G = new Graph(num_nodes);
    for (uint32_t i = 0; i < num_edges; i++) {
        new_srcs.push_back(edges[i].x);
        new_dests.push_back(edges[i].y);
    }
    auto perm = get_random_permutation(num_edges);

    gettimeofday(&t_start, &tzp);
    G->add_edge_batch(new_srcs.data(), new_dests.data(), num_edges, perm);
    gettimeofday(&t_end, &tzp);
    free(edges);
    new_srcs.clear();
    new_dests.clear();
    float size_gb = G->get_size() / (float) 1073741824;
    PRINT("Load Graph: Nodes: " << G->get_num_vertices() <<" Edges: " << G->get_num_edges() << " Size: " << size_gb << " GB");
    print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);
    PRINT("Throughput: " << G->get_num_edges() / (float) cal_time_elapsed(&t_start, &t_end));
    PRINT("================ Load Graph END ================");
}


template <class G>
double test_pr(G& GA, commandLine& P) {
    struct timeval start, end;
    struct timezone tzp;

    long maxiters = P.getOptionLongValue("-maxiters",10);
    auto gname = P.getOptionValue("-gname", "none");

    std::cout << "Running PR" << std::endl;

    gettimeofday(&start, &tzp);
    auto pr_edge_map = PR_S<double>(GA, maxiters);
    gettimeofday(&end, &tzp);
    free(pr_edge_map);
    return cal_time_elapsed(&start, &end);
}

template <class G>
double test_cc(G& GA, commandLine& P) {
    struct timeval start, end;
    struct timezone tzp;

    std::cout << "Running CC" << std::endl;
    auto gname = P.getOptionValue("-gname", "none");
    // with edge map
    gettimeofday(&start, &tzp);
    auto cc_result = CC(GA);
    gettimeofday(&end, &tzp);

    free(cc_result);
    return cal_time_elapsed(&start, &end);
}

template <class G>
double test_tc(G& GA, commandLine& P) {
    struct timeval start, end;
    struct timezone tzp;

    std::cout << "Running TC" << std::endl;
    auto gname = P.getOptionValue("-gname", "none");

    gettimeofday(&start, &tzp);
    auto count = TC(GA);
    gettimeofday(&end, &tzp);
    return cal_time_elapsed(&start, &end);
}


// return time elapsed
template <class G>
double test_bfs(G& GA, commandLine& P, int trial) {
    struct timeval start, end;
    struct timezone tzp;

    long src = P.getOptionLongValue("-src",9);
    auto gname = P.getOptionValue("-gname", "none");

    std::cout << "Running BFS from source = " << src << std::endl;

    gettimeofday(&start, &tzp);
    auto bfs_edge_map = BFS_with_edge_map(GA, src);
    gettimeofday(&end, &tzp);
    free(bfs_edge_map);

    return cal_time_elapsed(&start, &end);
}


template <class G>
double test_k_hop(G& GA, commandLine& P, int k = 2) {
    struct timeval start, end;
    struct timezone tzp;

    gettimeofday(&start, &tzp);
//    K_HOP(GA, k);
    long n = GA.get_num_vertices();
    uint32_t nsrc = n/20;
    srand(n);
    parallel_for(int i=0;i<nsrc;i++){
        auto rdsrc = rand()%n;
        for(Graph::NeighborIterator it_A(&GA, rdsrc);!it_A.done();++it_A) {
            uint32_t v = (*it_A);
            if(k==2)
                for(Graph::NeighborIterator it_B(&GA, v);!it_B.done();++it_B)
                    auto v2 = (*it_B);
        }
    }
    gettimeofday(&end, &tzp);
    return cal_time_elapsed(&start, &end);
}

template <class G>
double test_lp(G& GA, commandLine& P) {
    struct timeval start, end;
    struct timezone tzp;

    long maxiters = P.getOptionLongValue("-maxiters",10);
    auto gname = P.getOptionValue("-gname", "none");
    gettimeofday(&start, &tzp);
    LP(GA,maxiters);
    gettimeofday(&end, &tzp);
    return cal_time_elapsed(&start, &end);
}

template <class G>
double test_read(G& GA, commandLine& P) {
    struct timeval start, end;
    struct timezone tzp;
    auto r = random_aspen();
    long n = GA.get_num_vertices();
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
    gettimeofday(&start, &tzp);
    parallel_for(uint32_t i = 0; i < updates; i++) {
        GA.is_edge(new_srcs[i], new_dests[i]);
    }
    gettimeofday(&end, &tzp);
    return cal_time_elapsed(&start, &end);
}




template <class Graph>
double execute(Graph& G, commandLine& P, std::string testname, int i) {
    if (testname == "BFS") {
        return test_bfs(G, P,i );
    } else if (testname == "PR") {
        return test_pr(G, P);
    } else if (testname == "CC") {
        return test_cc(G, P);
    } else if (testname == "TC") {
        return test_tc(G, P);
    } else if (testname == "1-HOP") {
        return test_k_hop(G, P , 1);
    } else if (testname == "2-HOP"){
        return test_k_hop(G, P, 2);
    } else if(testname == "LP"){
        return test_lp(G, P);
    } else if (testname == "Read") {
        return test_read(G, P);
    } else {
        std::cout << "Unknown test: " << testname << ". Quitting." << std::endl;
        exit(0);
    }
}


void run_algorithm(commandLine& P) {
    PRINT("=============== Run Algorithm BEGIN ===============");
    Graph Ga = *G;

    std::vector<std::string> test_ids;
    test_ids = {"Read","1-HOP","2-HOP","BFS","PR","CC","LP","TC"};
//    test_ids = {"Read"};//"1-HOP","2-HOP",

    size_t rounds = P.getOptionLongValue("-rounds", 1);
    auto gname = P.getOptionValue("-gname", "none");
    auto thd_num = P.getOptionLongValue("-core", 1);
    auto log = P.getOptionValue("-log", "none");
    std::ofstream alg_file(log, ios::app);

    for (auto test_id : test_ids) {
        std::vector<double> total_time;
        for (size_t i=0; i<rounds; i++) {
            double tm = execute(Ga, P, test_id, i);

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


// -gname livejournal -core 16 -f ../../../data/ADJgraph/livejournal.adj -log ../../../log/terrace/alg.log
// -gname orkut -core 16 -f ../../../data/ADJgraph/orkut.adj -log ../../../log/terrace/alg.log
// -gname livejournal -core 16 -f ../../../data/ADJgraph/livejournal.adj -log ../../../log/terrace/alg.log
int main(int argc, char** argv) {
    srand(time(NULL));

    commandLine P(argc, argv );
    auto thd_num = P.getOptionLongValue("-core", 1);
    set_num_workers(thd_num);
    printf("Running Terrace using %ld threads.\n", thd_num );

    load_graph(P);

    run_algorithm(P);

    del_graph();
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}
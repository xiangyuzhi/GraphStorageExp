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
    auto pr_edge_map = PR_S<double>(GA, maxiters, gname);
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
    auto cc_result = CC(GA,gname);
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
    auto count = TC(GA, gname);
    gettimeofday(&end, &tzp);
    return cal_time_elapsed(&start, &end);
}


// return time elapsed
template <class G>
double test_bfs(G& GA, commandLine& P, int trial) {
    struct timeval start, end;
    struct timezone tzp;

    long src = P.getOptionLongValue("-src",-1);
    auto gname = P.getOptionValue("-gname", "none");

    if (src == -1) {
        std::cout << "Please specify a source vertex to run the BFS from" << std::endl;
        exit(0);
    }
    std::cout << "Running BFS from source = " << src << std::endl;

    gettimeofday(&start, &tzp);
    auto bfs_edge_map = BFS_with_edge_map(GA, src, gname);
    gettimeofday(&end, &tzp);
    free(bfs_edge_map);

    return cal_time_elapsed(&start, &end);
}


template <class G>
double test_k_hop(G& GA, commandLine& P, int k = 2) {
    struct timeval start, end;
    struct timezone tzp;

    //long src = P.getOptionLongValue("-src",-1);
    gettimeofday(&start, &tzp);
    K_HOP(GA, k);
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
    LP(GA,maxiters,gname);
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
    } else if (testname == "1-hop") {
        return test_k_hop(G, P , 1);
    } else if (testname == "2-hop"){
        return test_k_hop(G, P, 2);
    } else if(testname == "LP"){
        return test_lp(G, P);
    } else {
        std::cout << "Unknown test: " << testname << ". Quitting." << std::endl;
        exit(0);
    }
}


void run_algorithm(commandLine& P) {
    PRINT("=============== Run Algorithm BEGIN ===============");
    Graph Ga = *G;
    auto filename = P.getOptionValue("-f", "none");
    size_t rounds = P.getOptionLongValue("-rounds", 4);

    std::vector<std::string> test_ids;
    test_ids = {"BFS","PR","CC","TC","1-hop","2-hop","LP"};

    auto gname = P.getOptionValue("-gname", "none");
    std::ofstream alg_file("../../../log/terrace/alg.log",ios::app);
    alg_file << "GRAPH" << "\t"+gname <<"\t["<<getCurrentTime0()<<']'<<std::endl;
    auto thd_num = P.getOptionLongValue("-core", 1);
    alg_file << "Using threads :" << "\t"<<thd_num<<endl;

    for (auto test_id : test_ids) {
        std::vector<double> total_time;
        for (size_t i=0; i<rounds; i++) {
            double tm = execute(Ga, P, test_id, i);

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


// -src 9 -maxiters 10 -gname slashdot -core 1 -f ../../../data/slashdot.adj
// -src 9 -maxiters 10 -gname LiveJournal -core 4 -f ../../../data/ADJgraph/LiveJournal.adj
int main(int argc, char** argv) {
    srand(time(NULL));

    commandLine P(argc, argv );
    auto thd_num = P.getOptionLongValue("-core", 1);
//    omp_set_num_threads(thd_num);
    set_num_workers(thd_num);
    printf("Running Terrace using %ld threads.\n", thd_num );

    auto gname = P.getOptionValue("-gname", "none");
    mkdir(("../../../log/terrace/" + gname).c_str(),S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);

    load_graph(P);

    run_algorithm(P);

    del_graph();
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}
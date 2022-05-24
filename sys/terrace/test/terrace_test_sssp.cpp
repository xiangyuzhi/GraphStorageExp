//
// Created by zxy on 4/24/22.
//

#define ENABLE_LOCK 1
#define WEIGHTED 1
#define VERIFY 0
#define OPENMP 1

#include "terrace_test.h"
#include "BellmanFord.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

void load_graph_random_weighted(commandLine& P) {
    PRINT("=============== Load Graph BEGIN ===============");
    auto filename = P.getOptionValue("-f", "none");
    pair_uint *edges = get_edges_from_file_adj_sym(filename.c_str(), &num_edges, &num_nodes);
    G = new Graph(num_nodes);
    std::vector<uint32_t> new_wghts;
    for (uint32_t i = 0; i < num_edges; i++) {
        new_srcs.push_back(edges[i].x);
        new_dests.push_back(edges[i].y);
        new_wghts.push_back( i%7 + 1 );
    }
    auto perm = get_random_permutation(num_edges);

    gettimeofday(&t_start, &tzp);
    G->add_edge_batch(new_srcs.data(), new_dests.data(), new_wghts.data(), num_edges, perm);
    gettimeofday(&t_end, &tzp);
    free(edges);
    new_srcs.clear();
    new_dests.clear();
    new_wghts.clear();
    float size_gb = G->get_size() / (float) 1073741824;
    PRINT("Load Graph: Nodes: " << G->get_num_vertices() << " Edges: " << G->get_num_edges() << " Size: " << size_gb
                                << " GB");
    print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);
    PRINT("Throughput: " << G->get_num_edges() / (float) cal_time_elapsed(&t_start, &t_end));
    PRINT("================ Load Graph END ===============");
}



template <class G>
double test_sssp(G& GA, commandLine& P) {
    long maxiters = P.getOptionLongValue("-maxiters",10);
    auto gname = P.getOptionValue("-gname", "none");
    std::cout << "Running SSSP" << std::endl;

    gettimeofday(&t_start, &tzp);
    int32_t* ret = SSSP_BF(GA, maxiters, gname);
    free(ret);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

template <class Graph>
double execute(Graph& G, commandLine& P, std::string testname, int i) {
    if (testname == "SSSP") {
        return test_sssp(G, P);
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
    auto gname = P.getOptionValue("-gname", "none");
    std::vector<std::string> test_ids;
    // if testname is TC, include it, otherwise exclude it
    test_ids = {"SSSP"};

    std::ofstream alg_file("../../../log/terrace/alg.log",ios::app);
    auto thd_num = P.getOptionLongValue("-core", 1);
    alg_file << "Using threads :" << "\t"<<thd_num<<endl;

    for (auto test_id : test_ids) {
        std::vector<double> total_time;
        for (size_t i=0; i<rounds; i++) {
            double tm = execute(Ga, P, test_id, i);

            std::cout << "\ttest=" << test_id<< "\ttime=" << tm<< "\titeration=" << i << std::endl;
            total_time.emplace_back(tm);
        }
        double avg_time = cal_time(total_time);
        std::cout <<"["<<getCurrentTime0()<<']'<< "AVG"<< "\ttest=" << test_id<< "\ttime=" << avg_time << "\tgraph=" << filename << std::endl;
        alg_file <<"\t["<<getCurrentTime0()<<']'<< "AVG"<< "\ttest=" << test_id<< "\ttime=" << avg_time << std::endl;
    }
    alg_file.close();
    PRINT("=============== Run Algorithm END ===============");
}


// -src 9 -maxiters 10 -gname slashdot -core 1 -f ../../../data/ADJgraph/slashdot.adj
// -src 9 -gname LiveJournal -core 1 -f ../../../data/ADJgraph/LiveJournal.adj
int main(int argc, char** argv) {
    srand(time(NULL));

    commandLine P(argc, argv);

    auto thd_num = P.getOptionLongValue("-core", 1);
    omp_set_num_threads(thd_num);
    printf("Running Terrace using %ld threads.\n", thd_num);
    auto gname = P.getOptionValue("-gname", "none");
    mkdir(("../../../log/terrace/" + gname).c_str(),S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);

    load_graph_random_weighted(P);

    run_algorithm(P);

    del_graph();
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}
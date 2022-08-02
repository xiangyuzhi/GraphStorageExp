//
// Created by 15743 on 2022/8/1.
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

// return time elapsed
template <class G>
double test_bfs(G& GA, commandLine& P, int trial) {
    uint32_t bfs_src = 9;
    bool thread = P.getOption("-thread");
    if(!thread) {
        size_t n = GA.get_num_vertices();
        size_t maxdeg = 0;
        size_t maxid = 9;
        for(uint32_t i=1;i<n;i++){
            size_t src_degree = GA.degree(i);
            if(src_degree > maxdeg) {
                maxdeg = src_degree;
                maxid = i;
            }
        }
        bfs_src = maxid;
    }
    auto gname = P.getOptionValue("-gname", "none");

    std::cout << "Running BFS from source = " << bfs_src << std::endl;

    struct timeval start, end;
    struct timezone tzp;
    gettimeofday(&start, &tzp);
    auto bfs_edge_map = BFS_with_edge_map(GA, bfs_src);
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
    } else if (testname == "1-HOP") {
        return test_k_hop(G, P , 1);
    } else if (testname == "2-HOP"){
        return test_k_hop(G, P, 2);
    } else if (testname == "Read") {
        return test_read(G, P);
    } else {
        std::cout << "Unknown test: " << testname << ". Quitting." << std::endl;
        exit(0);
    }
}


void run_algorithm(commandLine& P, int thd_num, string gname) {
    PRINT("=============== Run Algorithm BEGIN ===============");
    Graph &Ga = *G;

    std::vector<std::string> test_ids;
    test_ids = {"BFS","PR","1-HOP","2-HOP","Read"};
    size_t rounds = P.getOptionLongValue("-rounds", 5);
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


void batch_ins_del_read(commandLine& P, int thd_num, string gname){
    PRINT("=============== Batch Insert BEGIN ===============");

    auto log = P.getOptionValue("-log","none");
    std::ofstream log_file(log, ios::app);

    Graph &Ga = *G;
    std::vector<uint32_t> update_sizes = {100000};
    auto r = random_aspen();
    auto update_times = std::vector<double>();
    size_t n_trials = 1;
    for (size_t us=0; us<update_sizes.size(); us++) {
        double avg_insert = 0;
        double avg_delete = 0;
        double avg_read = 0;
        std::cout << "Running batch size: " << update_sizes[us] << std::endl;

        if (update_sizes[us] < 10000000)
            n_trials = 20;
        else n_trials = 5;
        size_t updates_to_run = update_sizes[us];
        auto perm = get_random_permutation(updates_to_run);
        for (size_t ts=0; ts<n_trials; ts++) {
            uint32_t num_nodes = Ga.get_num_vertices();

            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            size_t nn = 1 << (log2_up(num_nodes) - 1);
            auto rmat = rMat<uint32_t>(nn, r.ith_rand(100+ts), a, b, c);
            for(uint32_t i = 0; i < updates_to_run; i++) {
                std::pair<uint32_t, uint32_t> edge = rmat(i);
                new_srcs.push_back(edge.first);
                new_dests.push_back(edge.second);
            }
            pair_uint *edges = (pair_uint*)calloc(updates_to_run, sizeof(pair_uint));
            for (uint32_t i = 0; i < updates_to_run; i++) {
                edges[i].x = new_srcs[i];
                edges[i].y = new_dests[i];
            }
            integerSort_y((pair_els*)edges, updates_to_run, num_nodes);
            integerSort_x((pair_els*)edges, updates_to_run, num_nodes);
            new_srcs.clear();
            new_srcs.clear();
            for (uint32_t i = 0; i < updates_to_run; i++) {
                new_srcs.push_back(edges[i].x);
                new_dests.push_back(edges[i].y);
            }

            gettimeofday(&t_start, &tzp);
            Ga.add_edge_batch(new_srcs.data(), new_dests.data(), updates_to_run, perm);
            gettimeofday(&t_end, &tzp);
            avg_insert += cal_time_elapsed(&t_start, &t_end);

            gettimeofday(&t_start, &tzp);
            for(uint32_t i = 0; i < updates_to_run; i++) {
                Ga.remove_edge(new_srcs[i], new_dests[i]);
            }
            gettimeofday(&t_end, &tzp);
            avg_delete +=  cal_time_elapsed(&t_start, &t_end);
        }
        double time_i = (double) avg_insert / n_trials;
        double insert_throughput = updates_to_run / time_i;
        printf("batch_size = %zu, average insert: %f, throughput %e\n", updates_to_run, time_i, insert_throughput);
        log_file<< gname<<","<<thd_num<<",e,insert,"<< update_sizes[us] <<","<<insert_throughput << "\n";

        double time_d = (double) avg_delete / n_trials;
        double delete_throughput = updates_to_run / time_d;
        printf("batch_size = %zu, average delete: %f, throughput %e\n", updates_to_run, time_d, delete_throughput);
        log_file<< gname<<"," <<thd_num<<",e,delete,"<< update_sizes[us] <<","<<delete_throughput << "\n";
    }
    PRINT("=============== Batch Insert END ===============");
}


// -gname livejournal -core 16 -f ../../../data/ADJgraph/livejournal.adj -log ../../../log/terrace/edge.log

int main(int argc, char** argv) {
    srand(time(NULL));

    commandLine P(argc, argv);
    bool thread = P.getOption("-thread");
    if(thread){
        {
            auto gname = P.getOptionValue("-gname", "none");
            load_graph(P);
            std::vector<uint32_t> threads = {1,4,8,12};
            for(auto thd_num : threads){
                set_num_workers(thd_num);
                cout << "Running Terrace using " << thd_num << " threads." << endl;
                run_algorithm(P, thd_num, gname);
            }

            for(auto thd_num: threads){
                set_num_workers(thd_num);
                cout << "Running Terrace using " << thd_num << " threads." << endl;
                batch_ins_del_read(P, thd_num, gname);
            }
            del_graph();
        }
    }

    else {
        auto insert_f = [&](uint32_t deg, uint32_t logv){
            uint32_t e = (1L<<logv)*deg;
            num_edges = e;
            cout<<"v: "<<logv<<" e: "<<deg<<endl;

            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            auto r = random_aspen();
            size_t nn = 1 << (log2_up(1L<<logv) - 1);
            auto rmat = rMat<uint32_t>(nn, r.ith_rand(100), a, b, c);
            G = new Graph((1L<<logv));
            new_srcs.clear();new_dests.clear();
            auto perm = get_random_permutation(e);
            for (int i = 0; i < e; i++){
                std::pair<uint32_t, uint32_t> edge = rmat(i);
                new_srcs.push_back(edge.first);
                new_dests.push_back(edge.second);
            }
            G->add_edge_batch(new_srcs.data(), new_dests.data(), e, perm);
            new_srcs.clear();new_dests.clear();

            string gname = "graph_"+to_string(logv)+"_"+to_string(deg);
            run_algorithm(P, 16, gname);
            batch_ins_del_read(P, 16, gname);
            del_graph();
        };

        {
            std::vector<uint32_t> vertices = {20,21,22,23,24,25,26};
            for(auto v : vertices){
                insert_f(30,v);
            }
        }

        {
            std::vector<uint32_t> edges = {10,20,30,40,50,60,70};
            for(auto e : edges){
                insert_f(e, 23);
            }
        }
    }
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}
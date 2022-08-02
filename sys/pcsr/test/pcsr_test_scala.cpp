//
// Created by 15743 on 2022/7/31.
//

#define OPENMP 1
#include "pcsr_test.h"
#include "BFS.h"
#include "PR.h"
#include "k-hop.h"
#include "parallel.h"

double test_bfs(commandLine& P) {
    uint32_t bfs_src = 9;
    bool thread = P.getOption("-thread");
    if(!thread) {
        uint64_t n = G->get_n();
        size_t maxdeg = 0;
        size_t maxid = 9;
        for(uint32_t i=1;i<n;i++){
            size_t src_degree = get_deg(G, i);
            if(src_degree > maxdeg) {
                maxdeg = src_degree;
                maxid = i;
            }
        }
        bfs_src = maxid;
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
    PR<double, PCSR>(G, maxiters);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double test_k_hop(commandLine& P, int k) {
    gettimeofday(&t_start, &tzp);
//    K_HOP(G, k);
    uint64_t n = G->get_n();
    uint32_t nsrc = n/20;
    srand(n);
    parallel_for(int i=0;i<nsrc;i++){
        auto rdsrc = rand()%n;
        uint32_t sta = G->nodes[rdsrc].beginning;
        uint32_t end = G->nodes[rdsrc].end;
        for (uint32_t i = sta + 1; i < end; i++) {
            uint32_t v = G->edges.items[i].dest;
            uint32_t w = G->edges.items[i].value;
            if(k==2) {
                uint32_t sta2 = G->nodes[v].beginning;
                uint32_t end2 = G->nodes[v].end;
                for (uint32_t j = sta2 + 1; j < end2; j++) {
                    uint32_t v2 = G->edges.items[j].dest;
                }
            }
        }
    }
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double test_read(commandLine& P) {
    auto r = random_aspen();
    uint64_t n = G->get_n();
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
    parallel_for(uint32_t i = 0; i < updates; i++) {
        G->find_value(new_srcs[i], new_dests[i]);
    }
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double execute(commandLine& P, string testname) {
    if (testname == "BFS") {
        return test_bfs(P);
    } else if (testname == "PR") {
        return test_pr(P);
    } else if (testname == "1-HOP") {
        return test_k_hop(P, 1);
    }else if (testname == "2-HOP") {
        return test_k_hop(P, 2);
    } else if (testname == "Read") {
        return test_read(P);
    } else {
        std::cout << "Unknown test: " << testname << ". Quitting." << std::endl;
        exit(0);
    }
}

void run_algorithm(commandLine& P, int thd_num, string gname) {
    PRINT("=============== Run Algorithm BEGIN ===============");

    std::vector<std::string> test_ids;
    //    test_ids = {"1-HOP","2-HOP","BFS","SSSP","PR","CC","LP","Read","TC"};
    test_ids = {"BFS","PR","1-HOP","2-HOP","Read"};

    size_t rounds = P.getOptionLongValue("-rounds", 5);
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


void batch_ins_del_read(commandLine& P, int thd_num, string gname){
    PRINT("=============== Batch Insert BEGIN ===============");

    auto log = P.getOptionValue("-log","none");
    std::ofstream log_file(log, ios::app);

    PCSR &Ga = *G;
    std::vector<uint32_t> update_sizes = {100000};//
    auto r = random_aspen();
    auto update_times = std::vector<double>();
    size_t n_trials = 1;
    for (size_t us=0; us<update_sizes.size(); us++) {
        Ga = *G;
        printf("GN: %u %u\n",Ga.get_n(),Ga.edges.N);
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
            uint32_t num_nodes = Ga.get_n();
            new_srcs.clear();
            new_dests.clear();

            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            size_t nn = 1 << (log2_up(num_nodes) - 1);
            auto rmat = rMat<uint32_t>(nn, r.ith_rand(100+ts), a, b, c);
            for( uint32_t i = 0; i < updates_to_run; i++) {
                std::pair<uint32_t, uint32_t> edge = rmat(i);
                new_srcs.push_back(edge.first);
                new_dests.push_back(edge.second);
            }

            gettimeofday(&t_start, &tzp);
            for (uint32_t i =0 ; i< updates_to_run;i++){
                Ga.add_edge_update(new_srcs[i],new_dests[i],1);
            }
            gettimeofday(&t_end, &tzp);
            avg_insert += cal_time_elapsed(&t_start, &t_end);

        }
        double time_i = (double) avg_insert / n_trials;
        double insert_throughput = updates_to_run / time_i;
        printf("batch_size = %zu, average insert: %f, throughput %e\n", updates_to_run, time_i, insert_throughput);
        log_file<< gname<<","<<thd_num<<",e,insert,"<< update_sizes[us] <<","<<insert_throughput << "\n";
    }
    PRINT("=============== Batch Insert END ===============");
}


// -src 9 -maxiters 5 -f ../../../data/slashdot.adj
// -gname LiveJournal -core 1 -f ../../../data/ADJgraph/LiveJournal.adj
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
                cout << "Running Aspen using " << thd_num << " threads." << endl;
                run_algorithm(P, thd_num, gname);
            }

            del_G();
        }
    }

    else {
        auto insert_f = [&](uint32_t deg, uint32_t logv){
            uint32_t e = (1L<<logv)*deg;
            num_edges = e;
            cout<<"v: "<<logv<<" e: "<<deg<<endl;
            G = new PCSR((1L<<logv));

            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            auto r = random_aspen();
            size_t nn = 1 << (log2_up(1L<<logv) - 1);
            auto rmat = rMat<uint32_t>(nn, r.ith_rand(100), a, b, c);

            for (uint32_t i =0 ; i< e;i++){
                std::pair<uint32_t, uint32_t> edge = rmat(i);
                G->add_edge(edge.first, edge.second, 1);
            }
            string gname = "graph_"+to_string(logv)+"_"+to_string(deg);
            run_algorithm(P, 16, gname);
            batch_ins_del_read(P, 16, gname);
            del_G();
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


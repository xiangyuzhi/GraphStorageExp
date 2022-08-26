//
// Created by 15743 on 2022/7/31.
//

#define OPENMP 1
#define _OPENMP 1
#include "stinger_test.h"
#include "BFS.h"
#include "PR.h"
#include "k-hop.h"
#include <thread>
#include "parallel.h"

double test_bfs(commandLine& P) {
    uint32_t bfs_src = 9;
    bool thread = P.getOption("-thread");
    if(!thread) {
        uint64_t n = G->max_nv;
        size_t maxdeg = 0;
        size_t maxid = 9;
        for(uint32_t i=1;i<n;i++){
            size_t src_degree = stinger_outdegree_get(G, i);
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
    PR<double, stinger>(G, maxiters);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}


double test_k_hop(commandLine& P, int k) {
    gettimeofday(&t_start, &tzp);
//    K_HOP(G, k);
    uint64_t n = G->max_nv;
    uint32_t nsrc = n/20;
    srand(n);
    parallel_for(int i=0;i<nsrc;i++){
        auto rdsrc = rand()%n;
        STINGER_FORALL_OUT_EDGES_OF_VTX_BEGIN(G, rdsrc) {
            uint64_t v = STINGER_EDGE_DEST;
            uint32_t w = *(reinterpret_cast<uint32_t *>(&(STINGER_EDGE_WEIGHT)));
            if(k==2){
                STINGER_FORALL_OUT_EDGES_OF_VTX_BEGIN(G, v) {
                    uint64_t v = STINGER_EDGE_DEST;
                }
                STINGER_FORALL_OUT_EDGES_OF_VTX_END();
            }
        }
        STINGER_FORALL_OUT_EDGES_OF_VTX_END();
    }
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double test_read(commandLine& P) {
    auto r = random_aspen();
    uint64_t n = G->max_nv;
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
        stinger_edge_touch(G,new_srcs[i], new_dests[i],0,0);
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

    size_t rounds = P.getOptionLongValue("-rounds", 5);
    auto log = P.getOptionValue("-log", "none");
    std::ofstream alg_file(log, ios::app);

    std::vector<std::string> test_ids;
    test_ids = {"BFS","PR","1-HOP","2-HOP","Read"};

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


template<typename graph>
void insert_edges(graph *GA, std::vector<uint32_t> &new_srcs, std::vector<uint32_t> &new_dests, int num_threads){
    auto routine_insert_edges = [&](int thread_id, uint64_t start, uint64_t length){
        for(int64_t pos = start, end = start + length; pos < end; pos++){
            stinger_insert_edge(GA, 0,new_srcs[pos] , new_dests[pos], 1, 0);
        }
    };
    int64_t edges_per_thread = new_srcs.size() / num_threads;
    int64_t odd_threads = new_srcs.size() % num_threads;
    vector<thread> threads;
    int64_t start = 0;
    for(int thread_id = 0; thread_id < num_threads; thread_id ++){
        int64_t length = edges_per_thread + (thread_id < odd_threads);
        threads.emplace_back(routine_insert_edges, thread_id, start, length);
        start += length;
    }
    for(auto& t : threads) t.join();
    threads.clear();
}

template<typename graph>
void delete_edges(graph *GA, std::vector<uint32_t> &new_srcs, std::vector<uint32_t> &new_dests, int num_threads){
    auto routine_insert_edges = [&](int thread_id, uint64_t start, uint64_t length){
        for(int64_t pos = start, end = start + length; pos < end; pos++){
            if(new_srcs[pos] != new_dests[pos])
                stinger_remove_edge (GA, 0 ,new_srcs[pos] , new_dests[pos] );
        }
    };
    int64_t edges_per_thread = new_srcs.size() / num_threads;
    int64_t odd_threads = new_srcs.size() % num_threads;
    vector<thread> threads;
    int64_t start = 0;
    for(int thread_id = 0; thread_id < num_threads; thread_id ++){
        int64_t length = edges_per_thread + (thread_id < odd_threads);
        threads.emplace_back(routine_insert_edges, thread_id, start, length);
        start += length;
    }
    for(auto& t : threads) t.join();
    threads.clear();
}



void batch_ins_del_read(commandLine& P, int thd_num, string gname){
    PRINT("=============== Batch Insert BEGIN ===============");

    auto log = P.getOptionValue("-log","none");
    std::ofstream log_file(log, ios::app);

    stinger &Ga = *G;
    std::vector<uint32_t> update_sizes = {100000};//10, 100, 1000 ,10000,,1000000, 10000000
    auto r = random_aspen();
    auto update_times = std::vector<double>();
    size_t n_trials = 1;
    for (size_t us=0; us<update_sizes.size(); us++) {
        //printf("GN: %u %u\n",Ga.max_nv,Ga.edges.N);
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
            uint32_t num_nodes = Ga.max_nv;
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
//            std::vector<update> updates;
//            for (int i = 0; i < updates_to_run; i++){
//                update u = {
//                        0, // type
//                        new_srcs[i], // source
//                        new_dests[i], // destination
//                        1, // weight
//                        int(ts)*100, // time
//                        0 // result
//                };
//                updates.push_back(u);
//            }
            gettimeofday(&t_start, &tzp);
            insert_edges(G, new_srcs, new_dests, thd_num);
            //stinger_batch_insert_edges<update>(G, updates.begin(), updates.end());
            gettimeofday(&t_end, &tzp);
            avg_insert += cal_time_elapsed(&t_start, &t_end);

            cout<<"insert over"<<endl;
            gettimeofday(&t_start, &tzp);
            delete_edges(G, new_srcs, new_dests, thd_num);
//            for(uint32_t i = 0; i < updates_to_run; i++) {
//                if(new_srcs[i]!= new_dests[i])
//                    stinger_remove_edge (G, 0 ,new_srcs[i] , new_dests[i] );
//            }
            gettimeofday(&t_end, &tzp);
            avg_delete +=  cal_time_elapsed(&t_start, &t_end);
            cout<<"delete over"<<endl;
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


// -thread -gname orkut -core 16 -f ../../../data/ADJgraph/orkut.adj -log ../../../log/stinger/scala.log
int main(int argc, char** argv) {
    srand(time(NULL));
    commandLine P(argc, argv);
    bool thread = P.getOption("-thread");
    if(thread){
        {
            auto gname = P.getOptionValue("-gname", "none");
            load_graph(P);
//            std::vector<uint32_t> threads = {1,4,8,12,16};
//            for(auto thd_num : threads){
//                set_num_workers(thd_num);
//                cout << "Running Stinger using " << thd_num << " threads." << endl;
//                run_algorithm(P, thd_num, gname);
//            }
//            for(auto thd_num: threads){
//                set_num_workers(thd_num);
//                cout << "Running Stinger using " << thd_num << " threads." << endl;
//                batch_ins_del_read(P, thd_num, gname);
//            }
            auto thd_num = P.getOptionLongValue("-core", 1);
            //set_num_workers(thd_num);
            cout << "Running Stinger using " << thd_num << " threads." << endl;
            batch_ins_del_read(P, thd_num, gname);
            del_G();
        }
    }

    else {
        auto insert_f = [&](uint32_t deg, uint32_t logv){
            uint32_t e = (1L<<logv)*deg;
            num_edges = e;
            cout<<"v: "<<logv<<" e: "<<deg<<endl;

            struct stinger_config_t * stinger_config;
            stinger_config = (struct stinger_config_t *)xcalloc(1,sizeof(struct stinger_config_t));
            stinger_config->nv = (1L<<logv);
            stinger_config->nebs = (1L<<logv)*10;
            stinger_config->netypes = 1;
            stinger_config->nvtypes = 1;
            stinger_config->memory_size = 1L<<38;
            G = stinger_new_full(stinger_config);
            xfree(stinger_config);

            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            auto r = random_aspen();
            size_t nn = 1 << (log2_up(1L<<logv) - 1);
            auto rmat = rMat<uint32_t>(nn, r.ith_rand(100), a, b, c);

            std::vector<update> updates;
            for (int i = 0; i < e; i++){
                std::pair<uint32_t, uint32_t> edge = rmat(i);
                update u = {
                        0, // type
                        edge.first, // source
                        edge.second, // destination
                        1, // weight
                        i*100, // time
                        0 // result
                };
                updates.push_back(u);
            }
            stinger_batch_insert_edges<update>(G, updates.begin(), updates.end());

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


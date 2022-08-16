//
// Created by 15743 on 2022/7/31.
//
#define OPENMP 1
#include "llama_test.h"
#include "BFS.h"
#include "PR.h"
#include "k-hop.h"
#include "parallel.h"
#include "thread"

template<typename Graph>
void add_edges(Graph &graph, vector<uint32_t> &new_srcs, vector<uint32_t> &new_dests, int num_threads){
    auto routine_insert_edges = [&graph, &new_srcs, &new_dests](int thread_id, uint64_t start, uint64_t length){
        for(int64_t pos = start, end = start + length; pos < end; pos++){
            while(1){
                try{
                    graph.tx_begin();
                    edge_t edge_id = graph.add_edge(new_srcs[pos], new_dests[pos]);
                    uint64_t w = 1;
                    graph.get_edge_property_64(g_llama_property_weights)->set(edge_id, *reinterpret_cast<uint64_t*>(&(w)));
                    graph.tx_commit();
                    break;
                }
                catch (exception e){
                    continue;
                }
            }
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

template<typename Graph>
void delete_edges(Graph &graph, vector<uint32_t> &new_srcs, vector<uint32_t> &new_dests, int num_threads){
    auto routine_insert_edges = [&graph, &new_srcs, &new_dests](int thread_id, uint64_t start, uint64_t length){
        for(int64_t pos = start, end = start + length; pos < end; pos++){
            while(1){
                try{
                    graph.tx_begin();
                    graph.delete_edge(new_srcs[pos], graph.find(new_srcs[pos], new_dests[pos]));
                    graph.tx_commit();
                    break;
                }
                catch (exception e){
                    continue;
                }
            }
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

double test_bfs(commandLine& P) {
    uint32_t bfs_src = 9;
    bool thread = P.getOption("-thread");
    if(!thread) {
        auto* g = get_snapshot(G);
        uint64_t n = g->max_nodes();
        size_t maxdeg = 0;
        size_t maxid = 9;
        for(uint32_t i=1;i<n;i++){
            size_t src_degree = g->out_degree(i);
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
    PR<double, ll_database>(G, maxiters);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double test_k_hop(commandLine& P, int k) {
    gettimeofday(&t_start, &tzp);
//    K_HOP(G, k);
    auto* g = get_snapshot(G);
    uint64_t n = g->max_nodes();
    uint32_t nsrc = n/20;
    srand(n);
    parallel_for(int i=0;i<nsrc;i++){
        auto rdsrc = rand()%n;
        ll_edge_iterator iter;
        g->out_iter_begin(iter, rdsrc);
        for (edge_t s_idx = g->out_iter_next(iter); s_idx != LL_NIL_EDGE; s_idx = g->out_iter_next(iter)) {
            node_t v = LL_ITER_OUT_NEXT_NODE(graph, iter, s_idx);
            uint32_t w = (uint32_t)get_out_edge_weight(g, s_idx);
            if(k==2){
                ll_edge_iterator iter2;
                g->out_iter_begin(iter2, v);
                for (edge_t j = g->out_iter_next(iter2); j != LL_NIL_EDGE; j = g->out_iter_next(iter2)) {
                    node_t v2 = LL_ITER_OUT_NEXT_NODE(graph, iter2, j);
                }
            }
        }
    }
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double test_read(commandLine& P) {
    auto r = random_aspen();
    auto* g = get_snapshot(G);
    uint64_t n = g->max_nodes();
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
        g->find(new_srcs[i], new_dests[i]);
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

void batch_ins_del_read(commandLine& P, int thd_num, string gname){
    PRINT("=============== Batch Insert BEGIN ===============");

    auto log = P.getOptionValue("-log","none");
    std::ofstream log_file(log, ios::app);

    ll_database Ga = *G;
    ll_writable_graph& graph = *Ga.graph();
    std::vector<uint32_t> update_sizes = {100000};//10, 100, 1000 ,10000,,1000000, 10000000
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
            add_edges(graph, new_srcs, new_dests, thd_num);
//            graph.tx_begin();
//            for (uint32_t i =0 ; i< updates_to_run;i++){
//                edge_t edge_id = graph.add_edge(new_srcs[i], new_dests[i]);
//                uint64_t w = 1;
//                graph.get_edge_property_64(g_llama_property_weights)->set(edge_id, *reinterpret_cast<uint64_t*>(&(w)));
//            }
//            graph.tx_commit();
            gettimeofday(&t_end, &tzp);
            avg_insert += cal_time_elapsed(&t_start, &t_end);

            gettimeofday(&t_start, &tzp);
            delete_edges(graph, new_srcs, new_dests, thd_num);
//            graph.tx_begin();
//            for(uint32_t i = 0; i < updates_to_run; i++) {
//                graph.delete_edge(new_srcs[i], graph.find(new_srcs[i], new_dests[i]));
//            }
//            graph.tx_commit();
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


// -gname livejournal -core 16 -f ../../../data/ADJgraph/livejournal.adj -log ../../../log/llama/edge.log
int main(int argc, char** argv) {
    srand(time(NULL));
    commandLine P(argc, argv);
    bool thread = P.getOption("-thread");
    if(thread){
        {
            auto gname = P.getOptionValue("-gname", "none");
            load_graph(P);
            std::vector<uint32_t> threads = {1,4,8,12};
//            for(auto thd_num : threads){
//                set_num_workers(thd_num);
//                cout << "Running LLAMA using " << thd_num << " threads." << endl;
//                run_algorithm(P, thd_num, gname);
//            }

            auto thd_num = P.getOptionLongValue("-core", 1);
            set_num_workers(thd_num);
            cout << "Running LLAMA using " << thd_num << " threads." << endl;
            batch_ins_del_read(P, thd_num, gname);
//            for(auto thd_num: threads){
//                set_num_workers(thd_num);
//                cout << "Running LLAMA using " << thd_num << " threads." << endl;
//                batch_ins_del_read(P, thd_num, gname);
//            }
            del_G();
        }
    }

    else {
        auto insert_f = [&](uint32_t deg, uint32_t logv){
            uint32_t e = (1L<<logv)*deg;
            cout<<"v: "<<logv<<" e: "<<deg<<endl;
            num_nodes = (1L<<logv);
            num_edges = e;
            char* database_directory = (char*) alloca(16);
            strcpy(database_directory, "db");

            G = new ll_database(database_directory);
            auto& csr = G->graph()->ro_graph();
            csr.create_uninitialized_edge_property_64(g_llama_property_weights, LL_T_DOUBLE);
            ll_writable_graph& graph = *G->graph();
            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            auto r = random_aspen();
            size_t nn = 1 << (log2_up(1L<<logv) - 1);
            auto rmat = rMat<uint32_t>(nn, r.ith_rand(100), a, b, c);
            new_srcs.clear();new_dests.clear();
            for (uint32_t i = 0; i < e; i++) {
                std::pair<uint32_t, uint32_t> edge = rmat(i);
                new_srcs.push_back(edge.first);
                new_dests.push_back(edge.second);
            }
            gettimeofday(&t_start, &tzp);
            graph.tx_begin();
            for (uint32_t i =0 ; i< e;i++){
                edge_t edge_id = graph.add_edge(new_srcs[i], new_dests[i]);
                uint32_t w = 1;
                graph.get_edge_property_64(g_llama_property_weights)->set(edge_id, *reinterpret_cast<uint32_t*>(&(w)));
                if(i%(e/10)==1){
                    graph.checkpoint();
                }
            }
            graph.checkpoint();
            graph.tx_commit();
            new_srcs.clear();
            new_dests.clear();
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

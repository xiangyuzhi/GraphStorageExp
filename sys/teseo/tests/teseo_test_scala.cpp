//
// Created by 15743 on 2022/8/1.
//

#define OPENMP 1
#include "teseo_test.h"
#include "BFS.h"
#include "PR.h"
#include "TC.h"
#include "k-hop.h"

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
    OpenMP &omp = *OMP;
    uint32_t bfs_src = 9;
    bool thread = P.getOption("-thread");
    if(!thread) {
        uint64_t n = omp.transaction().num_vertices();
        size_t maxdeg = 0;
        size_t maxid = 9;
        for(uint32_t i=1;i<n;i++){
            size_t src_degree = omp.transaction().degree(i, /* logical */ false);
            if(src_degree > maxdeg) {
                maxdeg = src_degree;
                maxid = i;
            }
        }
        bfs_src = maxid;
    }
    std::cout << "Running BFS from source = " << bfs_src << std::endl;

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

double test_tc(commandLine& P) {
    OpenMP &omp = *OMP;
    gettimeofday(&t_start, &tzp);
    auto count = TC(omp);
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
        uint32_t src = rand()%n;
        ompp.iterator().edges(src, /* logical ? */ false, [&](uint32_t v, double wgh){
            uint32_t w = (uint32_t)wgh;
            if(k==2){
                ompp.iterator().edges((uint32_t)v,/* logical ? */ false, [&](uint64_t v2, double wgh2){});
            }
        });
    }
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
    } else if (testname == "1-HOP") {
        return test_k_hop(P, 1);
    }else if (testname == "2-HOP") {
        return test_k_hop(P, 2);
    } else if (testname == "Read") {
        return test_read(P);
    } else if (testname == "TC") {
        return test_tc(P);
    } else {
        std::cout << "Unknown test: " << testname << ". Quitting." << std::endl;
        exit(0);
    }
}

void run_algorithm(commandLine& P, int thd_num, string gname) {
    PRINT("=============== Run Algorithm BEGIN ===============");

    std::vector<std::string> test_ids;
    test_ids = {"BFS","PR","1-HOP","2-HOP","Read","TC"};

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


template<typename graph>
void insert_edges(graph &GA, std::vector<uint32_t> &new_srcs, std::vector<uint32_t> &new_dests, int num_threads){
    auto routine_insert_edges = [&GA, &new_srcs, &new_dests](int thread_id, uint64_t start, uint64_t length){
        GA.register_thread();
        for(int64_t pos = start, end = start + length; pos < end; pos++){
            while(1){
                try{
                    auto tx = GA.start_transaction();
                    if(new_srcs[pos]!= new_dests[pos] && !tx.has_edge(new_srcs[pos], new_dests[pos])) {
                        tx.insert_edge(new_srcs[pos], new_dests[pos], 1.0);
                        tx.commit();
                    }
                    break;
                }
                catch (exception e){
                    continue;
                }
            }
        }
        GA.unregister_thread();
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
void delete_edges(graph &GA, std::vector<uint32_t> &new_srcs, std::vector<uint32_t> &new_dests, int num_threads){
    auto routine_insert_edges = [&GA, &new_srcs, &new_dests](int thread_id, uint64_t start, uint64_t length){
        GA.register_thread();
        for(int64_t pos = start, end = start + length; pos < end; pos++){
            while(1){
                try{
                    auto tx = GA.start_transaction();
                    if(new_srcs[pos]!= new_dests[pos] && tx.has_edge(new_srcs[pos], new_dests[pos])){
                        tx.remove_edge(new_srcs[pos], new_dests[pos]);
                        tx.commit();
                    }
                    break;
                }
                catch (exception e){
                    continue;
                }
            }
        }
        GA.unregister_thread();
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

    teseo::Teseo &Ga = *G;

    std::vector<uint32_t> update_sizes = {100000};
    auto r = random_aspen();
    auto update_times = std::vector<double>();
    size_t n_trials = 20;
    for (size_t us=0; us<update_sizes.size(); us++) {
        double avg_insert = 0;
        double avg_delete = 0;
        double avg_read = 0;
        std::cout << "Running batch size: " << update_sizes[us] << std::endl;

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
            insert_edges(Ga, new_srcs, new_dests, thd_num);
            gettimeofday(&t_end, &tzp);
            avg_insert += cal_time_elapsed(&t_start, &t_end);

            gettimeofday(&t_start, &tzp);
            delete_edges(Ga, new_srcs, new_dests, thd_num);
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


int main(int argc, char** argv) {
    srand(time(NULL));
    commandLine P(argc, argv);
    bool thread = P.getOption("-thread");

    if(thread){
        {
            auto gname = P.getOptionValue("-gname", "none");
            load_graph(P);
            std::vector<uint32_t> threads = {1,4,8,12,16};
            for(auto thd_num : threads){
                omp_set_num_threads(thd_num);
                cout << "Running Teseo using " << thd_num << " threads." << endl;
                run_algorithm(P, thd_num, gname);
            }

            for(auto thd_num: threads){
                omp_set_num_threads(thd_num);
                cout << "Running Teseo using " << thd_num << " threads." << endl;
                batch_ins_del_read(P, thd_num, gname);
            }

//            auto thd_num = P.getOptionLongValue("-core", 1);
//            cout << "Running Teseo using " << thd_num << " threads." << endl;
//            omp_set_num_threads(thd_num);
//            run_algorithm(P, thd_num, gname);
//            batch_ins_del_read(P, thd_num, gname);
            del_G();
        }
    }

    else {
        omp_set_num_threads(16);
        auto insert_f = [&](uint32_t deg, uint32_t logv){
            uint32_t e = (1L<<logv)*deg;
            num_edges = e;
            num_nodes = (1L<<logv);
            cout<<"v: "<<logv<<" e: "<<deg<<endl;

            G = new teseo::Teseo;
            for(uint64_t i = 0; i< num_nodes ;i++){
                auto tx1 = G->start_transaction();
                tx1.insert_vertex(i);
                tx1.commit();
            }

            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            auto r = random_aspen();
            size_t nn = 1 << (log2_up(1L<<logv) - 1);
            auto rmat = rMat<uint32_t>(num_nodes, r.ith_rand(100), a, b, c);

            for (int i = 0; i < e; i++){
                std::pair<uint32_t, uint32_t> edge = rmat(i);
                auto tx = G->start_transaction();
                if((edge.first!=edge.second) && !tx.has_edge(edge.first, edge.second)){
                    tx.insert_edge(edge.first, edge.second, /* weight */ 1.0);
                }
                tx.commit();
            }

            OMP = new OpenMP(G);

            string gname = "graph_"+to_string(logv)+"_"+to_string(deg);
            run_algorithm(P, 16, gname);
            batch_ins_del_read(P, 16, gname);
            del_G();
        };

        {
            auto v = P.getOptionIntValue("-v", -1);
            auto e = P.getOptionIntValue("-e", -1);
            insert_f(e, v);
        }
    }

    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}


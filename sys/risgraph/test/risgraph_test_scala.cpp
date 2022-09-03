//
// Created by 15743 on 2022/7/31.
//


#define CILK 1
#include "risgraph_test.h"
#include "BFS.h"
#include "PR.h"
#include "k-hop.h"

#include "utils/io_util.h"
#include "utils/rmat_util.h"
#include "type.hpp"
#include "io.hpp"
#include "storage.hpp"


void load_graph(commandLine& P){
    auto filename = P.getOptionValue("-f", "none");
    pair_uint *edges = get_edges_from_file_adj_sym(filename.c_str(), &num_edges, &num_nodes);

    G = new Graph<uint64_t> (num_nodes, num_edges, false, true);
    std::vector<int> vv = G->alloc_vertex_array<int>();
    G->fill_vertex_array<int>(vv, 1);

    std::pair<uint64_t, uint64_t> *raw_edges;
    raw_edges = new std::pair<uint64_t, uint64_t>[num_edges];
    for (uint32_t i = 0; i < num_edges; i++) {
        raw_edges[i].first = edges[i].x;
        raw_edges[i].second = edges[i].y;
    }

    auto perm = get_random_permutation(num_edges);
    PRINT("=============== Load Graph BEGIN ===============");
    gettimeofday(&t_start, &tzp);
    for(uint32_t i=0; i< num_edges; i++){
        const auto &e = raw_edges[i];
        G->add_edge({e.first, e.second, 1}, true);
    }

    gettimeofday(&t_end, &tzp);
    free(edges);
    free(raw_edges);
    new_srcs.clear();
    new_dests.clear();
    //float size_gb = G->get_size() / (float) 1073741824;
    //PRINT("Load Graph: Nodes: " << G->get_n() <<" Edges: " << G->edges.N<< " Size: " << size_gb << " GB");
    PRINT("Load Graph: Nodes: " << num_nodes <<" Edges: "<<num_edges);
    print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);
    PRINT("Throughput: " << num_nodes / (float) cal_time_elapsed(&t_start, &t_end));
    PRINT("================ Load Graph END ================");
}



double test_bfs(commandLine& P) {
    uint32_t bfs_src = 9;
    bool thread = P.getOption("-thread");
    if(!thread) {
        uint64_t n = G->get_vertex_num();
        size_t maxdeg = 0;
        size_t maxid = 9;
        for(uint32_t i=1;i<n;i++){
            size_t src_degree = G->get_outgoing_degree(i);
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
    PR<double, Graph<uint64_t> >(G, maxiters);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}


double test_k_hop(commandLine& P, int k) {
    gettimeofday(&t_start, &tzp);
//    K_HOP(G, k);
    uint64_t n = G->get_vertex_num();
    uint32_t nsrc = n/20;
    cout<<"nsrc "<<nsrc<<endl;
    srand(n);
    cilk_for(int i=0;i<nsrc;i++){
        auto rdsrc = rand()%n;
        auto tra = G->outgoing.get_adjlist_iter(rdsrc);
        for(auto iter=tra.first;iter!=tra.second;iter++) {
            auto edge = *iter;
            const uint64_t v = edge.nbr;
            if(k==2){
                auto tra2 = G->outgoing.get_adjlist_iter(v);
                for(auto iter2=tra2.first;iter2!=tra2.second;iter2++) {
                    auto edge2 = *iter2;
                    const uint64_t v2 = edge2.nbr;
                }
            }
        }
//        auto rdsrc = rand()%n;
//        for (auto e : G->outgoing.get_adjlist(rdsrc)) {
//            uint64_t v = e.nbr;
//            uint64_t w  = (uint64_t) e.data;
//            if(k==2){
//                for (auto e2 : G->outgoing.get_adjlist(v))
//                    uint64_t v2 = e2.nbr;
//            }
//        }
    }
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}


double test_read(commandLine& P) {
    auto r = random_aspen();
    int64_t n = G->get_vertex_num();
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
    cilk_for(uint32_t i = 0; i < updates; i++) {
        auto adjitr = G->get_outgoing_adjlist_range(new_srcs[i]);
        for(auto iter = adjitr.first ; iter != adjitr.second;iter++){
            auto edge = *iter;
            uint64_t dst = edge.nbr;
            if(dst == new_dests[i]) break;
        }
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
    test_ids = {"BFS","PR","Read","1-HOP","2-HOP"};//

    size_t rounds = P.getOptionLongValue("-rounds", 1);
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

    std::vector<uint32_t> update_sizes = {100000};//
    auto r = random_aspen();
    auto update_times = std::vector<double>();
    size_t n_trials = 1;
    for (size_t us=0; us<update_sizes.size(); us++) {
        printf("GN: %u %lu\n",num_nodes,num_edges);
        double avg_insert = 0;
        double avg_delete = 0;
        double avg_read = 0;
        std::cout << "Running batch size: " << update_sizes[us] << std::endl;

        if (update_sizes[us] < 10000000)
            n_trials = 1;
        else n_trials = 1;
        size_t updates_to_run = update_sizes[us];
        auto perm = get_random_permutation(updates_to_run);
        for (size_t ts=0; ts<n_trials; ts++) {
            uint32_t GN = num_nodes;
            new_srcs.clear();
            new_dests.clear();

            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            size_t nn = 1 << (log2_up(num_nodes) - 1);
            auto rmat = rMat<uint32_t>(nn, r.ith_rand(100+ts), a, b, c);

            std::pair<uint64_t, uint64_t> *raw_edges;
            raw_edges = new std::pair<uint64_t, uint64_t>[num_edges];
            for (uint32_t i = 0; i < updates_to_run; i++) {
                std::pair<uint32_t, uint32_t> edge = rmat(i);
                raw_edges[i].first = edge.first;
                raw_edges[i].second = edge.second;
            }
            gettimeofday(&t_start, &tzp);

            cilk_for(uint32_t i=0; i< updates_to_run; i++){
                const auto &e = raw_edges[i];
                G->add_edge({e.first, e.second}, true);
            }

            gettimeofday(&t_end, &tzp);
            avg_insert += cal_time_elapsed(&t_start, &t_end);

            gettimeofday(&t_start, &tzp);
            cilk_for(uint32_t i = 0; i < updates_to_run; i++) {
                const auto &e = raw_edges[i];
                G->del_edge({e.first, e.second}, true);
            }

            gettimeofday(&t_end, &tzp);
            avg_delete +=  cal_time_elapsed(&t_start, &t_end);

            free(raw_edges);
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


// -gname livejournal -thread -core 16 -f ../../../data/ADJgraph/livejournal.adj -log ../../../log/risgraph/edge.log
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
                set_num_workers(thd_num);
                cout << "Running Aspen using " << thd_num << " threads." << endl;
                run_algorithm(P, thd_num, gname);
            }
            for(auto thd_num: threads){
                set_num_workers(thd_num);
                cout << "Running Aspen using " << thd_num << " threads." << endl;
                batch_ins_del_read(P, thd_num, gname);
            }
            del_G();
        }
    }

    else {
        auto insert_f = [&](uint32_t deg, uint32_t logv){
            uint32_t e = (1L<<logv)*deg;
            num_edges = e;
            num_nodes =  (1L<<logv);
            cout<<"v: "<<logv<<" e: "<<deg<<endl;

            G = new Graph<uint64_t> ((1L<<logv), e, false, true);
            std::vector<int> vv = G->alloc_vertex_array<int>();
            G->fill_vertex_array<int>(vv, 1);

            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            auto r = random_aspen();
            size_t nn = 1 << (log2_up(1L<<logv) - 1);
            auto rmat = rMat<uint32_t>(num_nodes, r.ith_rand(100), a, b, c);
            for(uint32_t i=0; i< num_edges; i++){
                std::pair<uint32_t, uint32_t> edge = rmat(i);
                G->add_edge({edge.first, edge.second, 1}, true);
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


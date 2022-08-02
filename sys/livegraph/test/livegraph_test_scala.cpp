//
// Created by 15743 on 2022/7/31.
//

#define CILK 1
#include "livegraph_test.h"
#include "BFS.h"
#include "PR.h"
#include "parallel.h"
#include <thread>

template<typename Trans>
uint32_t pre_cal_max_deg(Trans &tx, uint64_t n){
    uint32_t ret = 9;
    uint32_t max_deg = 0;
    for(uint32_t i = 1; i < n; i++) {
        auto iterator = tx.get_edges(i, /* label */ 0);
        uint64_t cnt = 0;
        while(iterator.valid()){
            cnt ++;
            iterator.next();
        }
        if(cnt>max_deg){
            max_deg = cnt;
            ret = i;
        }
    }
    return ret;
}

double test_bfs(commandLine& P) {
    uint32_t bfs_src = 9;
    bool thread = P.getOption("-thread");
    if(!thread) {
        auto tx = G->begin_read_only_transaction();
        uint64_t n = G->get_max_vertex_id();
        bfs_src = pre_cal_max_deg(tx, n);
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
    PR<double>(G, maxiters);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double test_k_hop(commandLine& P, int k) {
    gettimeofday(&t_start, &tzp);
    //K_HOP(G, k);
    if(k==1){
        auto tx = G->begin_read_only_transaction();
        uint64_t n = G->get_max_vertex_id();
        uint32_t nsrc = n/20;
        srand(n);
        parallel_for(int i=0;i<nsrc;i++){
            auto src = rand()%n;
            for(auto iterator = tx.get_edges(src,0);iterator.valid();iterator.next()){
                uint64_t v = iterator.dst_id();
                uint32_t w = *reinterpret_cast<const uint32_t*>(iterator.edge_data().data());
            }
        }
    }
    else {
        auto tx = G->begin_read_only_transaction();
        uint64_t n = G->get_max_vertex_id();
        uint32_t nsrc = n/20;
        srand(n);
        parallel_for(int i=0;i<nsrc;i++){
            auto src = rand()%n;
            for(auto iterator = tx.get_edges(src,0);iterator.valid();iterator.next()){
                uint64_t v = iterator.dst_id();
                uint32_t w = *reinterpret_cast<const uint32_t*>(iterator.edge_data().data());
                for(auto it2 = tx.get_edges(v,0);it2.valid();it2.next()){
                    uint64_t v2 = it2.dst_id();
                }
            }
        }
    }
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

template<typename graph>
void read_edges(graph *GA, vector<uint32_t> &new_srcs, vector<uint32_t> &new_dests, int num_threads){
    auto routine_insert_edges = [GA, &new_srcs, &new_dests](int thread_id, uint64_t start, uint64_t length){
        for(int64_t pos = start, end = start + length; pos < end; pos++){
            while(1){
                try{
                    auto tx2 = G->begin_read_only_transaction();
                    vertex_dictionary_t::const_accessor accessor1, accessor2;
                    if(VertexDictionary->find(accessor1, new_srcs[pos]) && VertexDictionary->find(accessor2, new_dests[pos])){
                        lg::vertex_t internal_source_id = accessor1->second;
                        lg::vertex_t internal_destination_id = accessor2->second;
                        string_view lg_weight = tx2.get_edge(internal_source_id, /* label */ 0, internal_destination_id);
                    }
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


double test_read(commandLine& P) {
    auto thd_num = P.getOptionLongValue("-core", 1);
    auto r = random_aspen();
    uint64_t n = G->get_max_vertex_id();
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
    read_edges(G, new_srcs, new_dests, thd_num);
    gettimeofday(&t_end, &tzp);
    new_srcs.clear();new_dests.clear();
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


template<typename graph>
void add_edges(graph *GA, vector<uint32_t> &new_srcs, vector<uint32_t> &new_dests, int num_threads){
    auto routine_insert_edges = [GA, &new_srcs, &new_dests](int thread_id, uint64_t start, uint64_t length){
        for(int64_t pos = start, end = start + length; pos < end; pos++){
            while(1){
                try{
                    auto tx1 = GA->begin_transaction();
                    vertex_dictionary_t::const_accessor accessor1, accessor2;
                    VertexDictionary->find(accessor1, new_srcs[pos]);
                    VertexDictionary->find(accessor2, new_dests[pos]);
                    lg::vertex_t internal_source_id = accessor1->second;
                    lg::vertex_t internal_destination_id = accessor2->second;
                    int w = 1;string_view weight { (char*) &w, sizeof(w) };
                    tx1.put_edge(internal_source_id, /* label */ 0, internal_destination_id, weight);
                    tx1.commit();
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

template<typename graph>
void delete_edges(graph *GA, vector<uint32_t> &new_srcs, vector<uint32_t> &new_dests, int num_threads){
    auto routine_insert_edges = [GA, &new_srcs, &new_dests](int thread_id, uint64_t start, uint64_t length){
        for(int64_t pos = start, end = start + length; pos < end; pos++){
            while(1){
                try{
                    vertex_dictionary_t::const_accessor accessor1, accessor2;
                    auto tx3 = G->begin_transaction();
                    VertexDictionary->find(accessor1, new_srcs[pos]);
                    VertexDictionary->find(accessor2, new_dests[pos]);
                    lg::vertex_t internal_source_id = accessor1->second;
                    lg::vertex_t internal_destination_id = accessor2->second;
                    tx3.del_edge(internal_source_id, /* label */ 0, internal_destination_id);
                    tx3.commit();
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


void batch_ins_del_read(commandLine& P, int thd_num, string gname){
    PRINT("=============== Batch Insert BEGIN ===============");

    auto log = P.getOptionValue("-log","none");
    std::ofstream log_file(log, ios::app);

    std::vector<uint32_t> update_sizes = {100000};//10, 100, 1000 ,10000,100000,1000000, 10000000
    auto r = random_aspen();
    auto update_times = std::vector<double>();
    size_t n_trials = 1;
    for (size_t us=0; us<update_sizes.size(); us++) {
        printf("GN: %lu \n",G->get_max_vertex_id() );
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
            uint64_t GN = G->get_max_vertex_id();
            new_srcs.clear();
            new_dests.clear();

            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            size_t nn = 1 << (log2_up(GN) - 1);
            auto rmat = rMat<uint32_t>(nn, r.ith_rand(100+ts), a, b, c);
            for( uint32_t i = 0; i < updates_to_run; i++) {
                std::pair<uint32_t, uint32_t> edge = rmat(i);
                new_srcs.push_back(edge.first);
                new_dests.push_back(edge.second);
            }


            // insert edge
            gettimeofday(&t_start, &tzp);
            add_edges(G, new_srcs, new_dests, thd_num);
            gettimeofday(&t_end, &tzp);
            avg_insert += cal_time_elapsed(&t_start, &t_end);

            // del edge
            gettimeofday(&t_start, &tzp);
            delete_edges(G, new_srcs, new_dests, thd_num);
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


// -gname livejournal -core 16 -f ../../../data/ADJgraph/livejournal.adj -log ../../../log/livegraph/edge.log
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
                cout << "Running LiveGraph using " << thd_num << " threads." << endl;
                run_algorithm(P, thd_num, gname);
            }

            for(auto thd_num: threads){
                set_num_workers(thd_num);
                cout << "Running LiveGraph using " << thd_num << " threads." << endl;
                batch_ins_del_read(P, thd_num, gname);
            }
            del_G();
        }
    }

    else {
        auto insert_f = [&](uint32_t deg, uint32_t logv){
            G = new lg::Graph();
            m_pHashMap = new tbb::concurrent_hash_map<uint64_t, /* vertex_t */ uint64_t>();
            auto tx = G->begin_transaction();
            for(uint64_t id=0; id<(1L<<logv); id++){
                vertex_dictionary_t::accessor accessor; // xlock
                VertexDictionary->insert(accessor, id);
                internal_id = tx.new_vertex();
                string_view data {};
                tx.put_vertex(internal_id, data);
                accessor->second = internal_id;
            }
            tx.commit();

            uint32_t e = (1L<<logv)*deg;
            num_edges = e;
            cout<<"v: "<<logv<<" e: "<<deg<<endl;
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
            auto tx2 = G->begin_transaction();
            vertex_dictionary_t::const_accessor accessor1, accessor2;  // shared lock on the dictionary
            parallel_for (uint32_t i =0 ; i< e;i++){
                VertexDictionary->find(accessor1, new_srcs[i]);
                VertexDictionary->find(accessor2, new_dests[i]);
                lg::vertex_t internal_source_id = accessor1->second;
                lg::vertex_t internal_destination_id = accessor2->second;
                int w = 1;string_view weight { (char*) &w, sizeof(w) };
                tx2.put_edge(internal_source_id, /* label */ 0, internal_destination_id, weight);
            }
            tx2.commit();

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
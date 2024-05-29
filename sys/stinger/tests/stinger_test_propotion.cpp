#include "stinger_test.h"
#include <thread>


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
void insert_read(graph *GA, std::vector<uint32_t> &new_srcs, std::vector<uint32_t> &new_dests, std::vector<uint32_t> &query_srcs, std::vector<uint32_t> &query_dests, int num_threads){
    auto routine_insert_edges = [&](int thread_id, uint64_t start, uint64_t length){
        // for(int64_t pos = start, end = start + length; pos < end; pos++){
        //     stinger_insert_edge(GA, 0,new_srcs[pos], new_dests[pos], 1, 0);
        //     stinger_edge_touch(GA, query_srcs[pos], query_dests[pos], 0, 0);
        // }

        // for(int64_t pos = start, end = start + length; pos < end; pos++){
        //     stinger_insert_edge(GA, 0,new_srcs[pos], new_dests[pos], 1, 0);
        //     for(int64_t n = 0; n < 9; n++){
        //         stinger_edge_touch(GA, query_srcs[pos*9+n], query_dests[pos*9+n], 0, 0);
        //     }
        // }

        for(int64_t pos = start, end = start + length; pos < end; pos++){
            for(int64_t n = 0; n < 9; n++){
                stinger_insert_edge(GA, 0,new_srcs[pos*9+n], new_dests[pos*9+n], 1, 0);
            }
            stinger_edge_touch(GA, query_srcs[pos], query_dests[pos], 0, 0);
        }
    };


    int64_t smaller_size = new_srcs.size() < query_srcs.size() ? new_srcs.size() : query_srcs.size();
    int64_t edges_per_thread = smaller_size / num_threads;
    int64_t odd_threads = smaller_size % num_threads;
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



void batch_ins_del_read(commandLine& P){
    PRINT("=============== Batch Insert BEGIN ===============");

    auto gname = P.getOptionValue("-gname", "none");
    auto thd_num = P.getOptionLongValue("-core", 1);
    auto log = P.getOptionValue("-log","none");
    std::ofstream log_file(log, ios::app);

    stinger &Ga = *G;
    // std::vector<uint32_t> update_sizes = {10, 100, 1000, 10000, 100000, 1000000, 10000000};
    std::vector<uint32_t> update_sizes = {900000};
    std::vector<uint32_t> update_sizes2 = {100000};
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
        size_t updates_to_run2 = update_sizes2[us];
        auto perm = get_random_permutation(updates_to_run);
        for (size_t ts=0; ts<n_trials; ts++) {
            uint32_t num_nodes = Ga.max_nv;
            new_srcs.clear();
            new_dests.clear();
            query_srcs.clear();
            query_dests.clear();

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

            // generate random deges from new_srcs and new_dests
            std::default_random_engine generator;
            std::uniform_int_distribution<size_t> distribution(0, new_srcs.size() - 1);
            for (size_t i = 0; i < updates_to_run2; i++) {
                size_t index = distribution(generator);
                query_srcs.push_back(new_srcs[index]);
                query_dests.push_back(new_dests[index]);
            }

            gettimeofday(&t_start, &tzp);
            insert_read(G, new_srcs, new_dests, query_srcs, query_dests, thd_num);
            gettimeofday(&t_end, &tzp);
            avg_insert += cal_time_elapsed(&t_start, &t_end);

            // delete
            delete_edges(G, new_srcs, new_dests, thd_num);
        }
        double time_i = (double) avg_insert / n_trials;
        double insert_throughput = (updates_to_run+updates_to_run2) / time_i;
        printf("batch_size = %zu, average insert: %f, throughput %e\n", updates_to_run, time_i, insert_throughput);
        log_file<< gname<<","<<thd_num<<",e,insert-read,"<< update_sizes[us] <<"-" << update_sizes2[us]<<","<<insert_throughput << "\n";
    }
    PRINT("=============== Batch Insert END ===============");
}


int main(int argc, char** argv) {
    srand(time(NULL));
    commandLine P(argc, argv);
    auto thd_num = P.getOptionLongValue("-core", 1);
    omp_set_num_threads(thd_num);

    printf("Running Stinger using %ld threads.\n", thd_num );
    load_graph(P);

    batch_ins_del_read(P);

    del_G();
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}
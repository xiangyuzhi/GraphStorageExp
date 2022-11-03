//
// Created by zxy on 5/8/22.
//

#include "teseo_test.h"


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

void batch_ins_del_read(commandLine& P){
    auto gname = P.getOptionValue("-gname", "none");
    auto thd_num = P.getOptionLongValue("-core", 1);
    auto log = P.getOptionValue("-log","none");
    std::ofstream log_file(log, ios::app);

    std::vector<uint32_t> update_sizes = {10,100,1000,10000,100000,1000000,10000000};
//    std::vector<uint32_t> update_sizes = {100000};
    std::vector<double> avg_insert, avg_delete;
    avg_insert.clear(); avg_delete.clear();
    for(size_t i=0; i<update_sizes.size(); i++) avg_insert.push_back(0.0), avg_delete.push_back(0.0);

    size_t n_trials = 20;
    for (size_t ts=0; ts<n_trials; ts++) {
        load_graph(P);
        teseo::Teseo &Ga = *G;

        auto r = random_aspen();
        auto update_times = std::vector<double>();
        PRINT("=============== Batch Insert BEGIN ===============");
        for (size_t us=0; us<update_sizes.size(); us++) {
            std::cout << "Running batch size: " << update_sizes[us] << std::endl;
            size_t updates_to_run = update_sizes[us];
            auto perm = get_random_permutation(updates_to_run);

            new_srcs.clear();new_dests.clear();
            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            size_t nn = 1 << (log2_up(num_nodes) - 1);
            auto rmat = rMat<uint32_t>(nn, r.ith_rand(100 + ts), a, b, c);
            for (uint32_t i = 0; i < updates_to_run; i++) {
                std::pair <uint32_t, uint32_t> edge = rmat(i);
                new_srcs.push_back(edge.first);
                new_dests.push_back(edge.second);
            }

            gettimeofday(&t_start, &tzp);
            insert_edges(Ga, new_srcs, new_dests, thd_num);
            gettimeofday(&t_end, &tzp);
            avg_insert[us] += cal_time_elapsed(&t_start, &t_end);

            gettimeofday(&t_start, &tzp);
            delete_edges(Ga, new_srcs, new_dests, thd_num);
            gettimeofday(&t_end, &tzp);
            avg_delete[us] += cal_time_elapsed(&t_start, &t_end);
        }
        PRINT("=============== Batch Insert END ===============");
    }

    for (size_t us=0; us<update_sizes.size(); us++){
        double time_i = (double) avg_insert[us] / n_trials;
        double insert_throughput = update_sizes[us] / time_i;
        printf("batch_size = %zu, average insert: %f, throughput %e\n", update_sizes[us], time_i, insert_throughput);
        log_file<< gname<<","<<thd_num<<",e,insert,"<< update_sizes[us] <<","<<insert_throughput << "\n";

        double time_d = (double) avg_delete[us] / n_trials;
        double delete_throughput = update_sizes[us] / time_d;
        printf("batch_size = %zu, average delete: %f, throughput %e\n", update_sizes[us], time_d, delete_throughput);
        log_file<< gname<<"," <<thd_num<<",e,delete,"<< update_sizes[us] <<","<<delete_throughput << "\n";
    }
}

// -gname slashdot -core 12 -f ../../../data/ADJgraph/slashdot.adj
int main(int argc, char** argv) {
    srand(time(NULL));
    commandLine P(argc, argv);
    auto thd_num = P.getOptionLongValue("-core", 1);
    omp_set_num_threads(thd_num);
    printf("Running Teseo using %ld threads.\n", thd_num );

    batch_ins_del_read(P);

    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}
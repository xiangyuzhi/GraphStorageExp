//
// Created by zxy on 5/8/22.
//

#include "llama_test.h"
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


void batch_ins_del_read(commandLine& P){
    PRINT("=============== Batch Insert BEGIN ===============");

    auto gname = P.getOptionValue("-gname", "none");
    auto thd_num = P.getOptionLongValue("-core", 1);
    auto log = P.getOptionValue("-log","none");
    std::ofstream log_file(log, ios::app);

    ll_database Ga = *G;
    ll_writable_graph& graph = *Ga.graph();
    std::vector<uint32_t> update_sizes = {10, 100, 1000 ,10000,100000,1000000, 10000000};//10, 100, 1000 ,10000,,1000000, 10000000
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

            // insert edge
            gettimeofday(&t_start, &tzp);
            add_edges(graph, new_srcs, new_dests, thd_num);
//            graph.checkpoint();
//            graph.tx_begin();
//            for (uint32_t i =0 ; i< updates_to_run;i++){
//                edge_t edge_id = graph.add_edge(new_srcs[i], new_dests[i]);
//                uint64_t w = 1;
//                graph.get_edge_property_64(g_llama_property_weights)->set(edge_id, *reinterpret_cast<uint64_t*>(&(w)));
//            }
//            graph.checkpoint();
//            graph.tx_commit();
            gettimeofday(&t_end, &tzp);
            avg_insert += cal_time_elapsed(&t_start, &t_end);

            // doesnpt implment del
            gettimeofday(&t_start, &tzp);
            delete_edges(graph, new_srcs, new_dests, thd_num);
//            graph.checkpoint();
//            graph.tx_begin();
//            for(uint32_t i = 0; i < updates_to_run; i++) {
//                graph.delete_edge(new_srcs[i], graph.find(new_srcs[i], new_dests[i]));
//            }
//            graph.checkpoint();
//            graph.tx_commit();
            gettimeofday(&t_end, &tzp);
            avg_delete +=  cal_time_elapsed(&t_start, &t_end);
        }
        double time_i = (double) avg_insert / n_trials;
        double insert_throughput = updates_to_run / time_i;
        printf("batch_size = %zu, average insert: %f, throughput %e\n", updates_to_run, time_i, insert_throughput);
        log_file<< gname<<","<<thd_num<<",e,insert,"<< update_sizes[us] <<","<<insert_throughput << "\n";

//        double time_r = (double) avg_read / n_trials;
//        double read_throughput = updates_to_run / time_r;
//        printf("batch_size = %zu, average read: %f, throughput %e\n", updates_to_run, time_r, read_throughput);
//        log_file<< gname<<","<<thd_num<<",e,read,"<< update_sizes[us] <<","<<read_throughput << "\n";

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
    auto thd_num = P.getOptionLongValue("-core", 1);
    printf("Running LLAMA using %ld threads.\n", thd_num );
    load_graph(P);

    batch_ins_del_read(P);

    del_G();
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}
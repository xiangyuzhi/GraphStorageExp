//
// Created by zxy on 5/7/22.
//
#define OPENMP 1
#include "graph1_test.h"
#include "omp.h"
#include "util/parallel.h"

void graph1_remove_e(uint64_t src, uint64_t dst){
    common::SpinLock& mutex = m_edge_locks[(src + dst) % m_num_edge_locks].m_lock;
    scoped_lock<common::SpinLock> xlock(mutex);
    do_update(/* is insert ? */ false, src, dst, 1.0);
}


//template<typename Graph>
//bool find_edge2(Graph *G, uint64_t v0, uint64_t v1)  {
//    uint64_t degree = G->get_degree_out(self_index);
//
//    lite_edge_t* neighbours = nullptr;
//    neighbours = (lite_edge_t*) realloc(neighbours, sizeof(neighbours[0]) * degree);
//    degree = G->get_nebrs_out(self_index, neighbours);
//}



void batch_ins_del_read(commandLine& P){
    PRINT("=============== Batch Insert BEGIN ===============");

    auto gname = P.getOptionValue("-gname", "none");
    auto thd_num = P.getOptionLongValue("-core", 1);
    auto log = P.getOptionValue("-log","none");
    std::ofstream log_file(log, ios::app);

    std::vector<uint32_t> update_sizes = {10, 100, 1000 ,10000,100000,1000000, 10000000};//10, 100, 1000 ,10000,100000,1000000, 10000000
    auto r = random_aspen();
    auto update_times = std::vector<double>();
    size_t n_trials = 1;
    for (size_t us=0; us<update_sizes.size(); us++) {
        PRINT("v size: "<<num_vertices() <<" e size: "<<m_num_edges);
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
            uint64_t num_nodes = num_vertices();
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
            parallel_for (uint32_t i =0 ; i< updates_to_run;i++){
                graph1_add_e(new_srcs[i],new_dests[i]);
            }
            gettimeofday(&t_end, &tzp);
            avg_insert += cal_time_elapsed(&t_start, &t_end);

            gettimeofday(&t_start, &tzp);
            parallel_for(uint32_t i = 0; i < updates_to_run; i++) {
                graph1_remove_e(new_srcs[i], new_dests[i]);
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


// -gname livejournal -core 16 -f ../../../data/ADJgraph/livejournal.adj -log ../../../log/graphone/edge.log
// -gname orkut -core 16 -f ../../../data/ADJgraph/orkut.adj -log ../../../log/graphone/edge.log
int main(int argc, char** argv) {
    srand(time(NULL));
    commandLine P(argc, argv);
    auto thd_num = P.getOptionLongValue("-core", 1);
    omp_set_num_threads(thd_num);
    printf("Running GraphOne using %ld threads.\n", thd_num );
    load_graph(P);

    batch_ins_del_read(P);

    del_g();

    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}
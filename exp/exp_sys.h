//
// Created by 15743 on 2022/6/9.
//
#ifndef EXP_EXP_SYS_H
#define EXP_EXP_SYS_H
#include "utils/rmat_util.h"
#include "utils/util.h"


template<typename Graph>
void batch_ins_read_v(commandLine& P, Graph G){
    std::vector<uint32_t> vertex_sizes = {10, 100, 1000 ,10000,100000,1000000,10000000};
    PRINT("=============== Load&Read Graph BEGIN ===============");

    auto log = P.getOptionValue("-log","none");
    std::ofstream alg_file(log,ios::app);
    auto thd_num = P.getOptionLongValue("-core", 1);
    struct timeval t_start, t_end;
    struct timezone tzp;

    for (auto vsize : vertex_sizes){
        gettimeofday(&t_start, &tzp);
        G.init_graph(vsize);
        gettimeofday(&t_end, &tzp);
        print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);

        uint64_t n = G.num_vertices();
        float time = cal_time_elapsed(&t_start, &t_end);
        PRINT("Throughput: " <<  n/time);
        alg_file <<thd_num<<",v,insert,"<< vsize <<","<<n/time << "\n";

        gettimeofday(&t_start, &tzp);
        G.traverse_vertex(vsize);
        gettimeofday(&t_end, &tzp);
        print_time_elapsed("Read Vertex Cost: ", &t_start, &t_end);
        time = cal_time_elapsed(&t_start, &t_end);
        PRINT("Throughput: " <<  n/time);
        alg_file <<thd_num<<",v,read,"<< vsize <<","<<n/time << "\n";
        G.del();
    }
    PRINT("================ Load&Read Graph END ================");
}

template<typename Graph>
void batch_ins_del_read_e(commandLine& P, Graph G) {
    PRINT("=============== Batch Insert BEGIN ===============");

    auto gname = P.getOptionValue("-gname", "none");
    auto thd_num = P.getOptionLongValue("-core", 1);
    auto log = P.getOptionValue("-log", "none");

    std::ofstream edge_file(log, ios::app);

    G.load_graph(P);

    std::vector<uint32_t> update_sizes = {10, 100, 1000 ,10000, 100000, 1000000, 10000000};
    auto r = random_aspen();
    auto update_times = std::vector<double>();
    size_t n_trials = 1;
    struct timeval t_start, t_end;
    struct timezone tzp;
    for (size_t us=0; us<update_sizes.size(); us++) {

        double avg_insert = 0;
        double avg_delete = 0;
        double avg_read = 0;
        std::cout << "Running batch size: " << update_sizes[us] << std::endl;

        if (update_sizes[us] < 10000000)
            n_trials = 10;
        else n_trials = 3;
        size_t updates_to_run = update_sizes[us];
        auto perm = get_random_permutation(updates_to_run);
        for (size_t ts=0; ts<n_trials; ts++) {
            uint32_t GN = G.num_vertices();

            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            size_t nn = 1 << (log2_up(GN) - 1);
            auto rmat = rMat<uint32_t>(nn, r.ith_rand(100+ts), a, b, c);

            auto *raw_edges = new std::pair<uint64_t, uint64_t>[updates_to_run];
            for (uint32_t i = 0; i < updates_to_run; i++) {
                raw_edges[i] = rmat(i);
            }

            gettimeofday(&t_start, &tzp);
            G.add_edges(updates_to_run, raw_edges, nn);
            gettimeofday(&t_end, &tzp);
            avg_insert += cal_time_elapsed(&t_start, &t_end);

            gettimeofday(&t_start, &tzp);
            G.read_edges(updates_to_run, raw_edges);
            gettimeofday(&t_end, &tzp);
            avg_read += cal_time_elapsed(&t_start, &t_end);

            gettimeofday(&t_start, &tzp);
            G.del_edges(updates_to_run, raw_edges, nn);
            gettimeofday(&t_end, &tzp);
            avg_delete +=  cal_time_elapsed(&t_start, &t_end);

            free(raw_edges);
        }
        double time_i = (double) avg_insert / n_trials;
        double insert_throughput = updates_to_run / time_i;
        printf("batch_size = %zu, average insert: %f, throughput %e\n", updates_to_run, time_i, insert_throughput);
        edge_file<< gname<<","<<thd_num<<",e,insert,"<< update_sizes[us] <<","<<insert_throughput << "\n";

        double time_r = (double) avg_read / n_trials;
        double read_throughput = updates_to_run / time_r;
        printf("batch_size = %zu, average read: %f, throughput %e\n", updates_to_run, time_r, read_throughput);
        edge_file<< gname<<","<<thd_num<<",e,read,"<< update_sizes[us] <<","<<read_throughput << "\n";

        double time_d = (double) avg_delete / n_trials;
        double delete_throughput = updates_to_run / time_d;
        printf("batch_size = %zu, average delete: %f, throughput %e\n", updates_to_run, time_d, delete_throughput);
        edge_file<< gname<<"," <<thd_num<<",e,delete,"<< update_sizes[us] <<","<<delete_throughput << "\n";
    }
    G.del();
    PRINT("=============== Batch Insert END ===============");
}






#endif //EXP_EXP_SYS_H

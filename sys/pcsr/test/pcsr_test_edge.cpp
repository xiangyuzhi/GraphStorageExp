//
// Created by zxy on 5/4/22.
//

#include "pcsr_test.h"

void batch_ins_del_read(){
    PRINT("=============== Batch Insert BEGIN ===============");
    PCSR Ga = *G;
    std::vector<uint32_t> update_sizes = {10, 100, 1000 ,10000,100000};//,1000000,10000000
    auto r = random_aspen();
    auto update_times = std::vector<double>();
    size_t n_trials = 1;
    for (size_t us=0; us<update_sizes.size(); us++) {
        Ga = *G;
        printf("GN: %u %u\n",Ga.get_n(),Ga.edges.N);
        double avg_insert = 0;
        double avg_delete = 0;
        double avg_read = 0;
        std::cout << "Running batch size: " << update_sizes[us] << std::endl;

        if (update_sizes[us] <= 10000000)
            n_trials = 20;
        else n_trials = 5;
        size_t updates_to_run = update_sizes[us];
        auto perm = get_random_permutation(updates_to_run);
        for (size_t ts=0; ts<n_trials; ts++) {
            uint32_t num_nodes = Ga.get_n();
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
            for (uint32_t i =0 ; i< updates_to_run;i++){
                Ga.add_edge_update(new_srcs[i],new_dests[i],1);
            }
            gettimeofday(&t_end, &tzp);
            avg_insert += cal_time_elapsed(&t_start, &t_end);


            gettimeofday(&t_start, &tzp);
            for(uint32_t i = 0; i < updates_to_run; i++) {
                Ga.find_value(new_srcs[i], new_dests[i]);
            }
            gettimeofday(&t_end, &tzp);
            avg_read += cal_time_elapsed(&t_start, &t_end);

            // doesnpt implment del
//            gettimeofday(&t_start, &tzp);
//            for(uint32_t i = 0; i < updates_to_run; i++) {
//                Ga.(new_srcs[i], new_dests[i]);
//            }
//            gettimeofday(&t_end, &tzp);
//            avg_delete +=  cal_time_elapsed(&t_start, &t_end);
        }
        double time_i = (double) avg_insert / n_trials;
        double insert_throughput = updates_to_run / time_i;
        printf("batch_size = %zu, average insert: %f, throughput %e\n", updates_to_run, time_i, insert_throughput);

//        double time_d = (double) avg_delete / n_trials;
//        double delete_throughput = updates_to_run / time_d;
//        printf("batch_size = %zu, average delete: %f, throughput %e\n", updates_to_run, time_d, delete_throughput);

        double time_r = (double) avg_read / n_trials;
        double read_throughput = updates_to_run / time_r;
        printf("batch_size = %zu, average read: %f, throughput %e\n", updates_to_run, time_r, read_throughput);
    }
    PRINT("=============== Batch Insert END ===============");
}


// -src 9 -maxiters 5 -f ../../../data/slashdot.adj
// -src 9 -maxiters 5 -f ../../../data/orkut.adj
int main(int argc, char** argv) {
    srand(time(NULL));
//    printf("Num workers: %ld\n", getWorkers());
    commandLine P(argc, argv, "./graph_bm [-t testname -r rounds -f file");
    load_graph(P);

    batch_ins_del_read();

    del_G();
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}
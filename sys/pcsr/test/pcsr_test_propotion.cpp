
//
// Created by zxy on 5/4/22.
//

#include "pcsr_test.h"

void batch_ins_del_read(commandLine& P){
    PRINT("=============== Batch Insert BEGIN ===============");

    auto gname = P.getOptionValue("-gname", "none");
    auto thd_num = P.getOptionLongValue("-core", 1);
    auto log = P.getOptionValue("-log","none");
    std::ofstream log_file(log, ios::app);

    PCSR &Ga = *G;
    // std::vector<uint32_t> update_sizes = {10, 100, 1000 ,10000,100000,1000000, 10000000};
    std::vector<uint32_t> update_sizes = {900000};
    std::vector<uint32_t> update_sizes2 = {100000};
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

        if (update_sizes[us] < 10000000)
            n_trials = 20;
        else n_trials = 5;
        size_t updates_to_run = update_sizes[us];
        size_t updates_to_run2 = update_sizes2[us];
        auto perm = get_random_permutation(updates_to_run);
        for (size_t ts=0; ts<n_trials; ts++) {
            uint32_t num_nodes = Ga.get_n();
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

            // for (uint32_t i =0 ; i< 500000;i++){
            //     Ga.add_edge_update(new_srcs[i],new_dests[i],1);
            //     Ga.find_value(query_srcs[i], query_dests[i]);
            // }
            // for (uint32_t i =0 ; i< 100000;i++){
            //     Ga.add_edge_update(new_srcs[i],new_dests[i],1);
            //     for (uint32_t n = 0; n < 9; n++){
            //         Ga.find_value(query_srcs[i*9+n], query_dests[i*9+n]);
            //     }
            // }
            for (uint32_t i =0 ; i< 100000;i++){
                for (uint32_t n = 0; n < 9; n++){
                    Ga.add_edge_update(new_srcs[i*9+n], new_dests[i*9+n], 1);
                }
                Ga.find_value(query_srcs[i],query_dests[i]);
            }

            gettimeofday(&t_end, &tzp);
            avg_insert += cal_time_elapsed(&t_start, &t_end);

        }
        double time_i = (double) avg_insert / n_trials;
        double insert_throughput = (updates_to_run + updates_to_run2) / time_i;
        printf("batch_size = %zu, average insert: %f, throughput %e\n", updates_to_run, time_i, insert_throughput);
        log_file<< gname<<","<<thd_num<<",e,insert-read,"<< update_sizes[us] <<"-" << update_sizes2[us]<<","<<insert_throughput << "\n";
    }
    PRINT("=============== Batch Insert END ===============");
}


// -src 9 -maxiters 5 -f ../../../data/slashdot.adj
// -gname LiveJournal -core 1 -f ../../../data/ADJgraph/LiveJournal.adj
int main(int argc, char** argv) {
    srand(time(NULL));
    commandLine P(argc, argv);
    auto thd_num = P.getOptionLongValue("-core", 1);
    printf("Running PCSR using %ld threads.\n", thd_num );
    load_graph(P);

    batch_ins_del_read(P);

    del_G();
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}
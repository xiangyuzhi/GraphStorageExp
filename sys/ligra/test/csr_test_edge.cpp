//
// Created by zxy on 5/4/22.
//

#include "csr_test.h"

void batch_ins_del_read(commandLine& P){
    PRINT("=============== Batch Read BEGIN ===============");

    auto gname = P.getOptionValue("-gname", "none");
    std::ofstream alg_file("../../../log/ligra/edge.log",ios::app);
    alg_file << "GRAPH" << "\t"+gname <<"\t["<<getCurrentTime0()<<']'<<std::endl;
    auto thd_num = P.getOptionLongValue("-core", 1);
    alg_file << "Using threads :" << "\t"<<thd_num<<endl;

    std::vector<uint32_t> update_sizes = {10000000};//10, 100, 1000 ,10000,100000,1000000,
    auto r = random_aspen();
    auto update_times = std::vector<double>();
    size_t n_trials = 1;
    uint32_t GN = 100000;
    for (size_t us=0; us<update_sizes.size(); us++) {
        printf("GN: %u %lu\n", GN, update_sizes[us]);
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
            size_t nn = 1 << (log2_up(GN) - 1);
            auto rmat = rMat<uint32_t>(nn, r.ith_rand(100 + ts), a, b, c);
            std::vector<std::pair<uint32_t, std::pair<uint32_t, double> > > in_e;
            for (uint32_t i = 0; i < updates_to_run; i++) {
                std::pair<uint32_t, uint32_t> edge = rmat(i);
                new_srcs.push_back(edge.first);
                new_dests.push_back(edge.second);
                in_e.emplace_back(std::make_pair(edge.first, std::make_pair(edge.second, 1.0)));
            }
            G = new CSR(true, true);
            G->load(in_e, GN, update_sizes[us]);
            CSR &Ga = *G;
            gettimeofday(&t_start, &tzp);
            for (uint32_t i = 0; i < updates_to_run; i++) {
                Ga.get_weight(new_srcs[i], new_dests[i]);
            }
            gettimeofday(&t_end, &tzp);
            avg_read += cal_time_elapsed(&t_start, &t_end);

        }
        double time_r = (double) avg_read / n_trials;
        double read_throughput = updates_to_run / time_r;
        printf("batch_size = %zu, average read: %f, throughput %e\n", updates_to_run, time_r, read_throughput);
        alg_file <<"\t["<<getCurrentTime0()<<']' << "Read"<< "\tbatch_size=" << updates_to_run<< "\ttime=" << time_r<< "\tthroughput=" << read_throughput << std::endl;

        del_G();
    }
    PRINT("=============== Batch Insert END ===============");
}


// -src 9 -maxiters 5 -f ../../../data/slashdot.adj
// -gname LiveJournal -core 1 -f ../../../data/ADJgraph/LiveJournal.adj
int main(int argc, char** argv) {
    srand(time(NULL));
    commandLine P(argc, argv);
    auto thd_num = P.getOptionLongValue("-core", 1);
    printf("Running LiveGraph using %ld threads.\n", thd_num );
    batch_ins_del_read(P);

    del_G();
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}
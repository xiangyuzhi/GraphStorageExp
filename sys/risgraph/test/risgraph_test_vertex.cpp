//
// Created by zxy on 5/8/22.
//
//
// Created by zxy on 5/4/22.
//
#include "risgraph_test.h"

void batch_ins_read_v(commandLine& P){
    std::vector<uint32_t> vertex_sizes = {10, 100, 1000 ,10000,100000,1000000,10000000};
    PRINT("=============== Load&Read Graph BEGIN ===============");

    std::ofstream alg_file("../../../log/risgraph/vertex.log",ios::app);
    auto thd_num = P.getOptionLongValue("-core", 1);
    alg_file << "Using threads :" << "\t"<<thd_num<<endl;

    for (auto vsize : vertex_sizes){
        gettimeofday(&t_start, &tzp);
        Graph<void> G(vsize, 0, false, true);
        std::vector<int> vv = G.alloc_vertex_array<int>();
        G.fill_vertex_array<int>(vv, 1);
        gettimeofday(&t_end, &tzp);

        uint64_t n = vv.size();
        PRINT(">>> Load Graph Nodes: " << n );
        print_time_elapsed("Insert Vertex Cost: ", &t_start, &t_end);
        float time = cal_time_elapsed(&t_start, &t_end);
        PRINT("Throughput: " <<  n/time);
        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Insert"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;


        gettimeofday(&t_start, &tzp);
        for (uint64_t i = 0; i < vsize; i++)
            uint64_t a = G.get_outgoing_degree(i);
        gettimeofday(&t_end, &tzp);
        print_time_elapsed("Read Vertex Cost: ", &t_start, &t_end);
        time = cal_time_elapsed(&t_start, &t_end);
        PRINT("Throughput: " <<  n/time);
        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Read"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;


    }
    PRINT("================ Load&Read Graph END ================");
}

// -src 9 -maxiters 5 -f ../../../data/slashdot.adj
// -src 9 -maxiters 5 -f ../../../data/LiveJournal.adj
// -src 9 -maxiters 5 -f ../../../data/orkut.adj
int main(int argc, char** argv) {
    commandLine P(argc, argv);
    auto thd_num = P.getOptionLongValue("-core", 1);
    printf("Running RisGraph using %ld threads.\n", thd_num);
    batch_ins_read_v(P);
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}

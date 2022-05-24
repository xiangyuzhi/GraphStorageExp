//
// Created by zxy on 4/26/22.
//
#define OPENMP 1

#include "terrace_test.h"


void batch_ins_read_v(commandLine& P){

    std::vector<uint32_t> vertex_sizes = {10, 100, 1000 ,10000,100000,1000000,10000000};
    PRINT("=============== Load&Read Graph BEGIN ===============");

    std::ofstream alg_file("../../../log/terrace/vertex.log",ios::app);
    auto thd_num = P.getOptionLongValue("-core", 1);
    alg_file << "Using threads :" << "\t"<<thd_num<<endl;

    for (auto vsize : vertex_sizes){
        gettimeofday(&t_start, &tzp);
        G = new Graph(vsize);
        gettimeofday(&t_end, &tzp);
        float size_gb = G->get_size() / (float) 1073741824;
        PRINT("Load Graph: Nodes: " << G->get_num_vertices() << " Size: " << size_gb << " GB");
        print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);
        int n = G->get_num_vertices();
        float time = cal_time_elapsed(&t_start, &t_end);
        PRINT("Throughput: " <<  n/time);
        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Insert"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;

        gettimeofday(&t_start, &tzp);
        for (uint32_t i = 0; i < vsize; i++)
            G->degree(i);
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
    srand(time(NULL));
    commandLine P(argc, argv);
    auto thd_num = P.getOptionLongValue("-core", 1);
    omp_set_num_threads(thd_num);
    printf("Running Teseo using %ld threads.\n", getWorkers());

    batch_ins_read_v(P);
    del_graph();
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}

//
// Created by zxy on 5/7/22.
//
//#define CILK 1
#include "aspen_test.h"
#include <cstring>
#include <iostream>
using namespace std;
using edge_seq = pair<uintV, uintV>;

template <class Graph>
void print_stats(Graph& G) {
    Graph::print_stats();
}


void batch_ins_read_v(commandLine& P){
    std::vector<uint32_t> vertex_sizes = {10, 100, 1000 ,10000,100000,1000000,10000000};
    PRINT("=============== Load&Read Graph BEGIN ===============");

    std::ofstream alg_file("../../../log/aspen/vertex.log",ios::app);
    auto thd_num = P.getOptionLongValue("-core", 1);
    alg_file << "Using threads :" << "\t"<<thd_num<<endl;

    for (auto vsize : vertex_sizes){
        gettimeofday(&t_start, &tzp);
        auto VG = init_graph(vsize);
        auto S = VG.acquire_version();
        gettimeofday(&t_end, &tzp);

        size_t rep_size = S.graph.size_in_bytes();
        PRINT("Load Graph: Nodes: " << S.graph.num_vertices() << " Size: " << ((rep_size*1.0)/1024/1024/1024) << " GB");
        print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);
        int n = S.graph.num_vertices();
        float time = cal_time_elapsed(&t_start, &t_end);
        PRINT("Throughput: " <<  n/time);
        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Insert"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;


        gettimeofday(&t_start, &tzp);
        for (uint32_t i = 0; i < vsize; i++)
            auto a = S.graph.find_vertex(i);
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
    printf("Running Aspen using %ld threads.\n", thd_num);
    batch_ins_read_v(P);
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}

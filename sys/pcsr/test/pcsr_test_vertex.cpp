//
// Created by zxy on 5/4/22.
//
#include "PCSR.h"
#include "pcsr_test.h"

void batch_ins_read_v(){
    std::vector<uint32_t> vertex_sizes = {10, 100, 1000 ,10000,100000,1000000,10000000};
    PRINT("=============== Load&Read Graph BEGIN ===============");

    for (auto vsize : vertex_sizes){
        gettimeofday(&t_start, &tzp);
        PCSR pcsr = PCSR(vsize);
        gettimeofday(&t_end, &tzp);
        float size_gb = pcsr.get_size() / (float) 1073741824;
        PRINT("Load Graph: Nodes: " << pcsr.get_n() << " Size: " << size_gb << " GB");
        print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);
        PRINT("Throughput: " << pcsr.get_n() / (float) cal_time_elapsed(&t_start, &t_end));

        gettimeofday(&t_start, &tzp);
        for (uint32_t i = 0; i < vsize; i++)
            uint32_t a = pcsr.nodes[i].num_neighbors;
        gettimeofday(&t_end, &tzp);
        print_time_elapsed("Read Vertex Cost: ", &t_start, &t_end);
        PRINT("Throughput: " << pcsr.get_n() / (float) cal_time_elapsed(&t_start, &t_end));


//        gettimeofday(&t_start, &tzp);
//        for (uint32_t i = 0; i < vsize; i++)
//            pcsr.nodes.pop_back();
//        gettimeofday(&t_end, &tzp);
//        print_time_elapsed("Delete Vertex Cost: ", &t_start, &t_end);
//        PRINT("Throughput: " << pcsr.get_n() / (float) cal_time_elapsed(&t_start, &t_end));

    }
    PRINT("================ Load&Read Graph END ================");
}

// -src 9 -maxiters 5 -f ../../../data/slashdot.adj
// -src 9 -maxiters 5 -f ../../../data/LiveJournal.adj
// -src 9 -maxiters 5 -f ../../../data/orkut.adj
int main(int argc, char** argv) {
    srand(time(NULL));
    batch_ins_read_v();
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}

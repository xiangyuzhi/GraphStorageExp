//
// Created by zxy on 5/8/22.
//
//
// Created by zxy on 5/4/22.
//
#include "risgraph_test.h"

void batch_ins_read_v(){
    std::vector<uint32_t> vertex_sizes = {10, 100, 1000 ,10000,100000,1000000,10000000};
    PRINT("=============== Load&Read Graph BEGIN ===============");

    for (auto vsize : vertex_sizes){
        gettimeofday(&t_start, &tzp);
        Graph<void> G(vsize, 0, false, true);
//        G = new Graph<void> (vsize, 0, false, true);
        std::vector<int> vv = G.alloc_vertex_array<int>();
        G.fill_vertex_array<int>(vv, 1);
        gettimeofday(&t_end, &tzp);

//        float size_gb = pcsr.get_size() / (float) 1073741824;
//        PRINT("Load Graph: Nodes: " << pcsr.get_n() << " Size: " << size_gb << " GB");
        PRINT("Load Graph: Nodes: " << vv.size());
        print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);
        PRINT("Throughput: " << vv.size() / (float) cal_time_elapsed(&t_start, &t_end));

        gettimeofday(&t_start, &tzp);
        for (uint64_t i = 0; i < vsize; i++)
            uint64_t a = G.get_outgoing_degree(i);
        gettimeofday(&t_end, &tzp);
        print_time_elapsed("Read Vertex Cost: ", &t_start, &t_end);
        PRINT("Throughput: " << vv.size() / (float) cal_time_elapsed(&t_start, &t_end));


//        gettimeofday(&t_start, &tzp);
//        for (uint32_t i = 0; i < vsize; i++)
//            pcsr.nodes.pop_back();
//        gettimeofday(&t_end, &tzp);
//        print_time_elapsed("Delete Vertex Cost: ", &t_start, &t_end);
//        PRINT("Throughput: " << pcsr.get_n() / (float) cal_time_elapsed(&t_start, &t_end));
//        del_G();
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

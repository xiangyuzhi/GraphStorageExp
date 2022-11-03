//
// Created by zxy on 5/8/22.
//
#include "teseo_test.h"


void batch_ins_read_v(commandLine& P){
    std::vector<uint32_t> vertex_sizes = {10, 100, 1000 ,10000,100000,1000000,10000000};
    PRINT("=============== Load&Read Graph BEGIN ===============");

    std::ofstream alg_file("../../../log/teseo/vertex.log",ios::app);
    auto thd_num = P.getOptionLongValue("-core", 1);
    alg_file << "Using threads :" << "\t"<<thd_num<<endl;

    for (auto vsize : vertex_sizes){
        // insert vertex
        gettimeofday(&t_start, &tzp);
        G = new teseo::Teseo;
        auto tx1 = G->start_transaction();
        for(uint64_t i = 0; i< vsize ;i++)
            tx1.insert_vertex(i);
        uint64_t GN = tx1.num_vertices();
        tx1.commit();
        gettimeofday(&t_end, &tzp);

//        float size_gb = pcsr.get_size() / (float) 1073741824;
//        PRINT("Load Graph: Nodes: " << pcsr.get_n() << " Size: " << size_gb << " GB");
        PRINT("Load Graph: Nodes: " << GN );
        print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);
        int n = GN;
        float time = cal_time_elapsed(&t_start, &t_end);
        PRINT("Throughput: " <<  n/time);
        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Insert"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;

        gettimeofday(&t_start, &tzp);
        auto tx2 = G->start_transaction();
        for (uint64_t i = 0; i < vsize; i++){
            uint64_t vertex_id = tx2.vertex_id(i);
        }
        tx2.commit();
        gettimeofday(&t_end, &tzp);
        print_time_elapsed("Read Vertex Cost: ", &t_start, &t_end);
        time = cal_time_elapsed(&t_start, &t_end);
        PRINT("Throughput: " <<  n/time);
        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Read"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;


        gettimeofday(&t_start, &tzp);
        auto tx3 = G->start_transaction();
        for (uint32_t i = 0; i < vsize; i++)
            tx3.remove_vertex(i);
        tx3.commit();
        gettimeofday(&t_end, &tzp);
        print_time_elapsed("Delete Vertex Cost: ", &t_start, &t_end);
        time = cal_time_elapsed(&t_start, &t_end);
        PRINT("Throughput: " <<  n/time);
        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Delete"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;

    }
    PRINT("================ Load&Read Graph END ================");
}

// -src 9 -maxiters 5 -f ../../../data/slashdot.adj
// -src 9 -maxiters 5 -f ../../../data/LiveJournal.adj
// -core 16
int main(int argc, char** argv) {
    srand(time(NULL));
    commandLine P(argc, argv);
    auto thd_num = P.getOptionLongValue("-core", 1);
    omp_set_num_threads(thd_num);
    printf("Running Teseo using %ld threads.\n", getWorkers());

    batch_ins_read_v(P);
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}

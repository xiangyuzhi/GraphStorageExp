//
// Created by zxy on 5/6/22.
//
//
// Created by zxy on 4/25/22.
//
#include <iostream>
using  namespace  std;
#include "stinger_test.h"

#define printeb(eb) cout<<eb->vertexID<<' '<<eb->etype<<' '<<eb->high<<' '<<eb->smallStamp<<' '<<eb->largeStamp<<' '<<ebpool->ebpool + eb->next<<endl;



void batch_ins_read_v(commandLine& P){
    std::vector<uint32_t> vertex_sizes = {10, 100, 1000 ,10000,100000,1000000,10000000};
    PRINT("=============== Load&Read Graph BEGIN ===============");

    std::ofstream alg_file("../../../log/stinger/vertex.log",ios::app);
    auto thd_num = P.getOptionLongValue("-core", 1);
    alg_file << "Using threads :" << "\t"<<thd_num<<endl;

    for (auto vsize : vertex_sizes){
        gettimeofday(&t_start, &tzp);

        struct stinger_config_t * stinger_config;
        stinger_config = (struct stinger_config_t *)xcalloc(1,sizeof(struct stinger_config_t));
        stinger_config->nv = vsize;
        stinger_config->nebs = 0;
        stinger_config->netypes = 1;
        stinger_config->nvtypes = 1;
        stinger_config->memory_size = 1<<33;
        G = stinger_new_full(stinger_config);
        int64_t consistency = stinger_consistency_check(G,G->max_nv);
        cout<<consistency<<endl;
        xfree(stinger_config);
        gettimeofday(&t_end, &tzp);


        float size_gb = G->length / (float) 1073741824;
        uint64_t n = G->max_nv;
        PRINT("Load Graph Nodes: " << n << " Size: " << size_gb << " GB");
        print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);
        float time = cal_time_elapsed(&t_start, &t_end);
        PRINT("Throughput: " <<  n/time);
        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Insert"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;

        gettimeofday(&t_start, &tzp);
        for (uint32_t i = 0; i < vsize; i++)
            vweight_t weight = stinger_vweight_get(G,i);
        gettimeofday(&t_end, &tzp);
        print_time_elapsed("Read Vertex Cost: ", &t_start, &t_end);
        time = cal_time_elapsed(&t_start, &t_end);
        PRINT("Throughput: " <<  n/time);
        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Read"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;


        gettimeofday(&t_start, &tzp);
        for (uint32_t i = 0; i < vsize; i++)
            stinger_remove_vertex(G,i);
        gettimeofday(&t_end, &tzp);
        print_time_elapsed("Delete Vertex Cost: ", &t_start, &t_end);
        time = cal_time_elapsed(&t_start, &t_end);
        PRINT("Throughput: " <<  n/time);
        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Delete"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;

    }
    PRINT("================ Load&Read Graph END ================");
}


int main(int argc, char** argv){
    srand(time(NULL));
    commandLine P(argc, argv);
    auto thd_num = P.getOptionLongValue("-core", 1);
    printf("Running Stinger using %ld threads.\n", getWorkers());
    batch_ins_read_v(P);
    printf("!!!!! TEST OVER !!!!!\n");

    del_G();

}


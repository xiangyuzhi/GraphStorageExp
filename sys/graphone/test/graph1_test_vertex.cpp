//
// Created by zxy on 5/6/22.
//
#include "graph1_test.h"




void batch_ins_read_v(commandLine& P){
    std::vector<uint32_t> vertex_sizes = {10, 100, 1000 ,10000,100000,1000000,10000000};
    PRINT("=============== Load&Read Graph BEGIN ===============");

    std::ofstream alg_file("../../../log/graphone/vertex.log",ios::app);
    auto thd_num = P.getOptionLongValue("-core", 1);
    alg_file << "Using threads :" << "\t"<<thd_num<<endl;

    for (auto vsize : vertex_sizes){
        gettimeofday(&t_start, &tzp);
        g = new graph();
        graph1_init(vsize);
        for(uint64_t i=0;i<vsize;i++){
            graph1_add_v(i);
        }
        gettimeofday(&t_end, &tzp);
        PRINT("Batch size: " << vsize);
        print_time_elapsed("\tLoad Graph Cost: ", &t_start, &t_end);
        float time = cal_time_elapsed(&t_start, &t_end);
        auto* view = create_static_view(get_graphone_graph(), SIMPLE_MASK | PRIVATE_MASK);
        uint64_t n = view->v_count;
        PRINT("\tThroughput: " <<  n/time);
        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Insert"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;


        gettimeofday(&t_start, &tzp);
        for (uint32_t i = 0; i < vsize; i++){
            string str_vertex_id = to_string(i);
            sid_t logic_vertex_id = g->get_typekv()->get_sid(str_vertex_id.c_str());
        }
        gettimeofday(&t_end, &tzp);
        print_time_elapsed("\tRead Vertex Cost: ", &t_start, &t_end);
        time = cal_time_elapsed(&t_start, &t_end);
        PRINT("\tThroughput: " <<  n/time);
        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Read"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;


//        gettimeofday(&t_start, &tzp);
//        for (uint32_t i = 0; i < vsize; i++)
//            pcsr.nodes.pop_back();
//        gettimeofday(&t_end, &tzp);
//        print_time_elapsed("Delete Vertex Cost: ", &t_start, &t_end);
//        PRINT("Throughput: " << pcsr.get_n() / (float) cal_time_elapsed(&t_start, &t_end));

        del_g();
    }
    PRINT("================ Load&Read Graph END ================");
}


int main(int argc, char** argv) {
    commandLine P(argc, argv);
    batch_ins_read_v(P);
    printf("!!!!! TEST OVER !!!!!\n");
//    del_G();
}

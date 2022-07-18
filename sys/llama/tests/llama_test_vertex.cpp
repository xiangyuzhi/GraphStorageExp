//
// Created by zxy on 5/8/22.
//
#include "llama_test.h"


void batch_ins_read_v(commandLine& P){
    std::vector<node_t> vertex_sizes = {10, 100, 1000 ,10000,100000,1000000, 10000000};
    PRINT("=============== Load&Read Graph BEGIN ===============");

    std::ofstream alg_file("../../../log/llama/vertex.log",ios::app);
    auto thd_num = P.getOptionLongValue("-core", 1);
    alg_file << "Using threads :" << "\t"<<thd_num<<endl;

    char* database_directory = (char*) alloca(16);
    strcpy(database_directory, "db");

    for (auto vsize : vertex_sizes){
        // insert vertex
        gettimeofday(&t_start, &tzp);
        G = new ll_database(database_directory);
        ll_writable_graph& graph = *G->graph();
        graph.tx_begin();
        for(node_t i = 0; i< vsize ;i++)
            graph.add_node();
        graph.checkpoint();
        uint64_t n = graph.max_nodes();
        graph.tx_commit();
        gettimeofday(&t_end, &tzp);

//        float size_gb = pcsr.get_size() / (float) 1073741824;
//        PRINT("Load Graph: Nodes: " << pcsr.get_n() << " Size: " << size_gb << " GB");
        PRINT(">>> Load Graph Nodes: " << n );
        print_time_elapsed("\tInsert Vertex Cost: ", &t_start, &t_end);
        float time = cal_time_elapsed(&t_start, &t_end);

        PRINT("\tThroughput: " <<  n/time);
        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Insert"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;

        gettimeofday(&t_start, &tzp);
        graph.tx_begin();
        for (node_t i = 0; i < vsize; i++){
            //all nodes is from 0 - maxnode
            node_t vertex_id = graph.node_exists(i) ? i : -1;
        }
        graph.tx_commit();
        gettimeofday(&t_end, &tzp);
        print_time_elapsed("Read Vertex Cost: ", &t_start, &t_end);
        time = cal_time_elapsed(&t_start, &t_end);
        PRINT("\tThroughput: " <<  n/time);
        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Read"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;


        gettimeofday(&t_start, &tzp);
        graph.tx_begin();
        for (node_t i = 0; i < vsize; i++)
            graph.delete_node(i);
        graph.checkpoint();
        graph.tx_commit();
        gettimeofday(&t_end, &tzp);
        print_time_elapsed("\tDelete Vertex Cost: ", &t_start, &t_end);
        time = cal_time_elapsed(&t_start, &t_end);
        PRINT("\tThroughput: " <<  n/time);
        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Delete"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;

        delete(G);
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
    printf("Running LLAMA using %ld threads.\n", thd_num);
    batch_ins_read_v(P);
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}

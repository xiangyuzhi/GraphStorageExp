//
// Created by zxy on 5/7/22.
//
#include "livegraph_test.h"
#include "iostream"
using namespace std;

void batch_ins_read_v(commandLine& P){
    std::vector<uint32_t> vertex_sizes = {10, 100, 1000 ,10000}; //100000,1000000,10000000, for different stage
    PRINT("=============== Load&Read Graph BEGIN ===============");

    std::ofstream alg_file("../../../log/livegraph/vertex.log",ios::app);
    auto thd_num = P.getOptionLongValue("-core", 1);
    alg_file << "Using threads :" << "\t"<<thd_num<<endl;

    for (auto vsize : vertex_sizes){
        gettimeofday(&t_start, &tzp);

        G = new lg::Graph();
        m_pHashMap = new tbb::concurrent_hash_map<uint64_t, /* vertex_t */ uint64_t>();

        auto tx = G->begin_transaction();
        for(uint64_t id=0; id<vsize; id++){
            vertex_dictionary_t::accessor accessor; // xlock
            VertexDictionary->insert(accessor, id);
            internal_id = tx.new_vertex();
            string_view data {};
            tx.put_vertex(internal_id, data);
            accessor->second = internal_id;
        }
        tx.commit();
        gettimeofday(&t_end, &tzp);


//        float size_gb = pcsr.get_size() / (float) 1073741824;
//        PRINT("Load Graph: Nodes: " << G->get_max_vertex_id() << " Size: " << size_gb << " GB");
        auto txr = G->begin_read_only_transaction();
        uint64_t n = G->get_max_vertex_id();
        txr.abort();
        PRINT(">>> Load Graph Nodes: " << G->get_max_vertex_id() );
        float time = cal_time_elapsed(&t_start, &t_end);
        PRINT("Throughput: " <<  n/time);
        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Insert"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;


        gettimeofday(&t_start, &tzp);
        auto tx2 = G->begin_transaction();
        for (uint32_t i = 0; i < vsize; i++){
            vertex_dictionary_t::const_accessor accessor;
            VertexDictionary->find(accessor, i);
            lg::vertex_t internal_id = accessor->second;
            string_view data = tx2.get_vertex(internal_id);
        }
        tx2.commit();
        gettimeofday(&t_end, &tzp);
        print_time_elapsed("Read Vertex Cost: ", &t_start, &t_end);
        time = cal_time_elapsed(&t_start, &t_end);
        PRINT("Throughput: " <<  n/time);
        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Read"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;


        gettimeofday(&t_start, &tzp);
        vertex_dictionary_t::accessor accessor; // xlock
        auto tx3 = G->begin_transaction();
        for (uint32_t i = 0; i < vsize; i++){
            VertexDictionary->find(accessor, i);
            lg::vertex_t internal_id = accessor->second;
            tx3.del_vertex(internal_id);
            VertexDictionary->erase(accessor);
        }
        tx3.commit();
        gettimeofday(&t_end, &tzp);
        print_time_elapsed("Delete Vertex Cost: ", &t_start, &t_end);
        time = cal_time_elapsed(&t_start, &t_end);
        PRINT("Throughput: " <<  n/time);
        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Delete"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;

        del_G();
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
    printf("Running Livegraph using %ld threads.\n", getWorkers());
    batch_ins_read_v(P);
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}
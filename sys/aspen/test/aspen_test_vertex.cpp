//
// Created by zxy on 5/7/22.
//
//#define CILK 1
#include "aspen_test.h"
#include <cstring>
#include <iostream>
//#include "utils/rmat_util.h"
//#include "utils/util.h"
using namespace std;
using edge_seq = pair<uintV, uintV>;

template <class Graph>
void print_stats(Graph& G) {
    Graph::print_stats();
}

void print_time_elapsed(std::string desc, struct timeval* start, struct
        timeval* end)
{
    struct timeval elapsed;
    if (start->tv_usec > end->tv_usec) {
        end->tv_usec += 1000000;
        end->tv_sec--;
    }
    elapsed.tv_usec = end->tv_usec - start->tv_usec;
    elapsed.tv_sec = end->tv_sec - start->tv_sec;
    float time_elapsed = (elapsed.tv_sec * 1000000 + elapsed.tv_usec)/1000000.f;
    std::cout << desc << "Total Time Elapsed: " << std::to_string(time_elapsed) <<
              "seconds" << std::endl;
}

float cal_time_elapsed(struct timeval* start, struct timeval* end)
{
    struct timeval elapsed;
    if (start->tv_usec > end->tv_usec) {
        end->tv_usec += 1000000;
        end->tv_sec--;
    }
    elapsed.tv_usec = end->tv_usec - start->tv_usec;
    elapsed.tv_sec = end->tv_sec - start->tv_sec;
    return (elapsed.tv_sec * 1000000 + elapsed.tv_usec)/1000000.f;
}

std::vector<uint32_t> get_random_permutation(uint32_t num) {
    std::vector<uint32_t> perm(num);
    std::vector<uint32_t> vec(num);

    for (uint32_t i = 0; i < num; i++)
        vec[i] = i;

    uint32_t cnt{0};
    while (vec.size()) {
        uint32_t n = vec.size();
        srand(time(NULL));
        uint32_t idx = rand() % n;
        uint32_t val = vec[idx];
        std::swap(vec[idx], vec[n-1]);
        vec.pop_back();
        perm[cnt++] = val;
    }
    return perm;
}



void batch_ins_read_v(commandLine& P){
    std::vector<uint32_t> vertex_sizes = {10, 100, 1000 ,10000,100000,1000000,10000000};
    PRINT("=============== Load&Read Graph BEGIN ===============");

    std::ofstream alg_file("../../../log/aspen/vertex.log",ios::app);
    auto thd_num = P.getOptionLongValue("-core", 1);
//    alg_file << "Using threads :" << "\t"<<thd_num<<endl;

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
//        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Insert"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;
        alg_file <<thd_num<<",insert,"<< vsize <<","<<n/time << "\n";


        gettimeofday(&t_start, &tzp);
        for (uint32_t i = 0; i < vsize; i++)
            auto a = S.graph.find_vertex(i);
        gettimeofday(&t_end, &tzp);
        print_time_elapsed("Read Vertex Cost: ", &t_start, &t_end);
        time = cal_time_elapsed(&t_start, &t_end);
        PRINT("Throughput: " <<  n/time);
//        alg_file <<"\t["<<getCurrentTime0()<<']'<< "Read"<< "\tBatch size=" << vsize << "\tLatency=" << time/(float)n << "\tThroughput=" << n/time << std::endl;
        alg_file <<thd_num<<",read,"<< vsize <<","<<n/time << "\n";

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

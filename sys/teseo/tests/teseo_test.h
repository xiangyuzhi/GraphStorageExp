//
// Created by zxy on 5/8/22.
//

#ifndef EXP_TESEO_TEST_H
#define EXP_TESEO_TEST_H

#include <stdlib.h>
#include <assert.h>
#define NDEBUG
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <random>
#include <algorithm>
#include <queue>
#include "sys/time.h"
#include "io_util.h"
#include "util.h"
#include "rmat_util.h"
#include "omp.h"
#include "teseo.hpp"
#include "teseo_openmp.hpp"
using namespace std;

#define PRINT(x) do { \
	{ std::cout << x << std::endl; } \
} while (0)

teseo::Teseo *G;
OpenMP *OMP;
std::vector<uint32_t> new_srcs;
std::vector<uint32_t> new_dests;
std::vector<uint32_t> query_srcs;
std::vector<uint32_t> query_dests;
uint32_t num_nodes;
uint64_t num_edges;
std::string src, dest;
struct timeval t_start, t_end;
struct timezone tzp;

void del_G(){
    delete OMP; OMP = nullptr;
    delete G; G = nullptr;
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



struct commandLine {
    int argc;
    char** argv;
    std::string comLine;
    commandLine(int _c, char** _v, std::string _cl)
            : argc(_c), argv(_v), comLine(_cl) {
        if (getOption("-h") || getOption("-help"))
            badArgument();
    }

    commandLine(int _c, char** _v)
            : argc(_c), argv(_v), comLine("bad arguments") { }

    void badArgument() {
        std::cout << "usage: " << argv[0] << " " << comLine << std::endl;
        exit(0);
    }

    // get an argument
    // i is indexed from the last argument = 0, second to last indexed 1, ..
    char* getArgument(int i) {
        if (argc < 2+i) badArgument();
        return argv[argc-1-i];
    }

    // looks for two filenames
    std::pair<char*,char*> IOFileNames() {
        if (argc < 3) badArgument();
        return std::pair<char*,char*>(argv[argc-2],argv[argc-1]);
    }

    std::pair<size_t,char*> sizeAndFileName() {
        if (argc < 3) badArgument();
        return std::pair<size_t,char*>(std::atoi(argv[argc-2]),(char*) argv[argc-1]);
    }

    bool getOption(std::string option) {
        for (int i = 1; i < argc; i++)
            if ((std::string) argv[i] == option) return true;
        return false;
    }

    char* getOptionValue(std::string option) {
        for (int i = 1; i < argc-1; i++)
            if ((std::string) argv[i] == option) return argv[i+1];
        return NULL;
    }

    std::string getOptionValue(std::string option, std::string defaultValue) {
        for (int i = 1; i < argc-1; i++)
            if ((std::string) argv[i] == option) return (std::string) argv[i+1];
        return defaultValue;
    }

    long getOptionLongValue(std::string option, long defaultValue) {
        for (int i = 1; i < argc-1; i++)
            if ((std::string) argv[i] == option) {
                long r = atol(argv[i+1]);
                if (r < 0) badArgument();
                return r;
            }
        return defaultValue;
    }

    int getOptionIntValue(std::string option, int defaultValue) {
        for (int i = 1; i < argc-1; i++)
            if ((std::string) argv[i] == option) {
                int r = atoi(argv[i+1]);
                if (r < 0) badArgument();
                return r;
            }
        return defaultValue;
    }

    double getOptionDoubleValue(std::string option, double defaultValue) {
        for (int i = 1; i < argc-1; i++)
            if ((std::string) argv[i] == option) {
                double val;
                if (sscanf(argv[i+1], "%lf",  &val) == EOF) {
                    badArgument();
                }
                return val;
            }
        return defaultValue;
    }

};

double cal_time(std::vector<double> timelist){
    if(timelist.size() == 1) return timelist[0];
    sort(timelist.begin(),timelist.end());
    double st = 0.0;
    for(uint32_t i = 1 ;i < timelist.size()-1;i++)
        st += timelist[i];
    return st/double(timelist.size()-2);
}

static std::string getCurrentTime0() {
    std::time_t result = std::time(nullptr);
    std::string ret;
    ret.resize(64);
    int wsize = sprintf((char *)&ret[0], "%s", std::ctime(&result));
    ret.resize(wsize-1);
    return ret;
}


void test_graph(commandLine& P){
    auto filename = P.getOptionValue("-f", "none");
    pair_uint *edges = get_edges_from_file_adj_sym(filename.c_str(), &num_edges, &num_nodes);
    for (uint32_t i =0 ; i< num_edges;i++){
        cout<<edges[i].x<<' '<<edges[i].y<<endl;
    }
}


void load_graph_v2(commandLine& P){
    auto filename = P.getOptionValue("-f", "none");
    pair_uint *edges = get_edges_from_file_adj_sym(filename.c_str(), &num_edges, &num_nodes);

    PRINT("=============== Load Graph BEGIN ===============");
    gettimeofday(&t_start, &tzp);
    G = new teseo::Teseo;
    int cnt = 0;
    for (uint32_t i =0 ; i< num_edges;i++){
        if(i%(num_edges/100)==0){
            //cout<<cnt<<"%"<<endl;
            cnt++;
        }
        if(edges[i].x!=edges[i].y) {
            auto tx2 = G->start_transaction();
            if(!tx2.has_vertex(edges[i].x)) tx2.insert_vertex(edges[i].x);
            if(!tx2.has_vertex(edges[i].y)) tx2.insert_vertex(edges[i].y);
            tx2.insert_edge(edges[i].x, edges[i].y, /* weight */ 1.0);
            tx2.commit();
        }
    }
    auto txx = G->start_transaction(true);
    uint64_t GN = txx.num_vertices();
    uint64_t GM = txx.num_edges();
    gettimeofday(&t_end, &tzp);
    free(edges);

    OMP = new OpenMP(G);
    PRINT("Load Graph: Nodes: " << GN <<" Edges: " << GM);
    print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);
    PRINT("Throughput: " << GN / (float) cal_time_elapsed(&t_start, &t_end));
    PRINT("================ Load Graph END ================");
}

void load_graph(commandLine& P){
    auto filename = P.getOptionValue("-f", "none");
    pair_uint *edges = get_edges_from_file_adj_sym(filename.c_str(), &num_edges, &num_nodes);

    PRINT("=============== Load Graph BEGIN ===============");
    gettimeofday(&t_start, &tzp);
    G = new teseo::Teseo;
    for(uint64_t i = 0; i< num_nodes ;i++){
        auto tx1 = G->start_transaction();
        tx1.insert_vertex(i);
        tx1.commit();
    }
    int cnt = 0;
    for (uint32_t i =0 ; i< num_edges;i++){
        if(i%(num_edges/100)==0){
            //cout<<cnt<<"%"<<endl;
            cnt++;
        }
        if(edges[i].x!=edges[i].y) {
            auto tx2 = G->start_transaction();
            tx2.insert_edge(edges[i].x, edges[i].y, /* weight */ 1.0);
            tx2.commit();
        }
    }
    auto txx = G->start_transaction(true);
    uint64_t GN = txx.num_vertices();
    uint64_t GM = txx.num_edges();
    gettimeofday(&t_end, &tzp);
    free(edges);

    OMP = new OpenMP(G);
    PRINT("Load Graph: Nodes: " << GN <<" Edges: " << GM);
    print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);
    PRINT("Throughput: " << GN / (float) cal_time_elapsed(&t_start, &t_end));
    PRINT("================ Load Graph END ================");
}

//void load_graph(commandLine& P){
//    auto filename = P.getOptionValue("-f", "none");
//    pair_uint *edges = get_edges_from_file_adj_sym(filename.c_str(), &num_edges, &num_nodes);
//
//    G = new teseo::Teseo;
//    auto tx1 = G->start_transaction();
//    for(uint64_t i = 0; i< num_nodes ;i++)
//        tx1.insert_vertex(i);
//    tx1.commit();
//
//    PRINT("=============== Load Graph BEGIN ===============");
//    gettimeofday(&t_start, &tzp);
//
//    auto routine_insert_edges = [&](int thread_id, uint64_t start, uint64_t length){
//        G->register_thread();
//        for(int64_t pos = start, end = start + length; pos < end; pos++){
//            auto tx = G->start_transaction();
//            if(edges[pos].x!= edges[pos].y && !tx.has_edge(edges[pos].x, edges[pos].y)) {
//                tx.insert_edge(edges[pos].x, edges[pos].y, 1.0);
//            }
//            tx.commit();
//        }
//        G->unregister_thread();
//    };
//    int num_threads = P.getOptionLongValue("-core", 1);
//    int64_t edges_per_thread = num_edges / num_threads;
//    int64_t odd_threads = num_edges % num_threads;
//    vector<thread> threads;
//    int64_t start = 0;
//    for(int thread_id = 0; thread_id < num_threads; thread_id ++){
//        int64_t length = edges_per_thread + (thread_id < odd_threads);
//        threads.emplace_back(routine_insert_edges, thread_id, start, length);
//        start += length;
//    }
//    for(auto& t : threads) t.join();
//    threads.clear();
//
//    auto tx2 = G->start_transaction(true);
//    uint64_t GN = tx2.num_vertices();
//    uint64_t GM = tx2.num_edges();
//    tx2.commit();
//    gettimeofday(&t_end, &tzp);
//    free(edges);
//    OMP = new OpenMP(G);
//    PRINT("Load Graph: Nodes: " << GN <<" Edges: " << GM);
//    print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);
//    PRINT("Throughput: " << GN / (float) cal_time_elapsed(&t_start, &t_end));
//    PRINT("================ Load Graph END ================");
//}




#endif //EXP_TESEO_TEST_H

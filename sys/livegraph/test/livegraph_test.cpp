//
// Created by zxy on 4/25/22.
//

#define OPENMP 1
#include<livegraph.hpp>
#include<iostream>
#include "tbb/concurrent_hash_map.h"
using namespace std;
#include "parallel.h"
#include "omp.h"
#include<vector>
#include<thread>

using vertex_dictionary_t = tbb::concurrent_hash_map<uint64_t, lg::vertex_t>;
#define VertexDictionary reinterpret_cast<vertex_dictionary_t*>(m_pHashMap)
void* m_pHashMap;
lg::vertex_t internal_id = 0;
lg::Graph* G;

void init_graph(){
    G = new lg::Graph();
}
void del_G(){
    delete G;
    G = nullptr;
    delete VertexDictionary;
    m_pHashMap = nullptr;

}

void livegraph_add_e(uint64_t src, uint64_t dst){
    vertex_dictionary_t::const_accessor accessor1, accessor2;  // shared lock on the dictionary
    VertexDictionary->find(accessor1, src);
    VertexDictionary->find(accessor2, dst);
    lg::vertex_t internal_source_id = accessor1->second;
    lg::vertex_t internal_destination_id = accessor2->second;
    auto tx = G->begin_transaction();
    int w = 1;
    string_view weight { (char*) &w, sizeof(w) };
    tx.put_edge(internal_source_id, /* label */ 0, internal_destination_id, weight);
    tx.commit();
}

void add_vertex(uint64_t id){
    vertex_dictionary_t::accessor accessor; // xlock
    bool inserted = VertexDictionary->insert(accessor, id);
    auto tx = G->begin_transaction();
    internal_id = tx.new_vertex();
    cout<<internal_id<<endl;
    string_view data { (char*) &id, sizeof(id) };
    tx.put_vertex(internal_id, data);
    accessor->second = internal_id;
    tx.commit();

}

void test_newv(){

    auto tx = G->begin_transaction();
    for(int i=0; i<10; i++){
        lg::vertex_t t = tx.new_vertex();
        cout << t <<" "<<G->get_max_vertex_id()<<endl;
    }

}

void del_vertex(uint64_t id){
    vertex_dictionary_t::accessor accessor; // xlock
    bool found = VertexDictionary->find(accessor, id);
    lg::vertex_t internal_id = accessor->second;
    auto tx = G->begin_transaction();
    tx.del_vertex(internal_id);
    tx.commit();
    VertexDictionary->erase(accessor);
}


template<typename graph>
void update_edges(graph *GA, vector<uint32_t> &new_srcs, vector<uint32_t> &new_dests, int num_threads){
    auto routine_insert_edges = [GA, &new_srcs, &new_dests](int thread_id, uint64_t start, uint64_t length){
        for(int64_t pos = start, end = start + length; pos < end; pos++){
            while(1){
                try{
                    auto tx1 = GA->begin_transaction();
                    vertex_dictionary_t::const_accessor accessor1, accessor2;
                    VertexDictionary->find(accessor1, new_srcs[pos]);
                    VertexDictionary->find(accessor2, new_dests[pos]);
                    lg::vertex_t internal_source_id = accessor1->second;
                    lg::vertex_t internal_destination_id = accessor2->second;
                    int w = 1;string_view weight { (char*) &w, sizeof(w) };
                    tx1.put_edge(internal_source_id, /* label */ 0, internal_destination_id, weight);
                    tx1.commit();
                    break;
                }
                catch (exception e){
                    continue;
                }
            }
        }
    };
    int64_t edges_per_thread = new_srcs.size() / num_threads;
    int64_t odd_threads = new_srcs.size() % num_threads;
    vector<thread> threads;
    int64_t start = 0;
    for(int thread_id = 0; thread_id < num_threads; thread_id ++){
        int64_t length = edges_per_thread + (thread_id < odd_threads);
        threads.emplace_back(routine_insert_edges, thread_id, start, length);
        start += length;
    }
    for(auto& t : threads) t.join();
    threads.clear();
}


template<typename graph>
void read_edges(graph *GA, vector<uint32_t> &new_srcs, vector<uint32_t> &new_dests, int num_threads){
    auto routine_insert_edges = [GA, &new_srcs, &new_dests](int thread_id, uint64_t start, uint64_t length){
        for(int64_t pos = start, end = start + length; pos < end; pos++){
            while(1){
                try{
                    auto tx2 = G->begin_read_only_transaction();
                    vertex_dictionary_t::const_accessor accessor1, accessor2;
                    if(VertexDictionary->find(accessor1, new_srcs[pos]) && VertexDictionary->find(accessor2, new_dests[pos])){
                        lg::vertex_t internal_source_id = accessor1->second;
                        lg::vertex_t internal_destination_id = accessor2->second;
                        string_view lg_weight = tx2.get_edge(internal_source_id, /* label */ 0, internal_destination_id);
                    }
                    break;
                }
                catch (exception e){
                    continue;
                }
            }
        }
    };
    int64_t edges_per_thread = new_srcs.size() / num_threads;
    int64_t odd_threads = new_srcs.size() % num_threads;
    vector<thread> threads;
    int64_t start = 0;
    for(int thread_id = 0; thread_id < num_threads; thread_id ++){
        int64_t length = edges_per_thread + (thread_id < odd_threads);
        threads.emplace_back(routine_insert_edges, thread_id, start, length);
        start += length;
    }
    for(auto& t : threads) t.join();
    threads.clear();
}





int main(){
    G = new lg::Graph();
    m_pHashMap = new tbb::concurrent_hash_map<uint64_t, /* vertex_t */ uint64_t>();
    auto tx = G->begin_transaction();
    for(uint64_t id=0; id<10; id++){
        vertex_dictionary_t::accessor accessor; // xlock
        VertexDictionary->insert(accessor, id);
        internal_id = tx.new_vertex();
        string_view data {};
        tx.put_vertex(internal_id, data);
        accessor->second = internal_id;
    }
    tx.commit();


    vector<uint32_t> new_srcs;
    vector<uint32_t> new_dests;
    for(int i=2;i<10;i++){
        new_srcs.push_back(1);
        new_dests.push_back(i);
    }
    update_edges(G, new_srcs, new_dests, 4);
    read_edges(G, new_srcs, new_dests, 4);
//    auto tx1 = G->begin_transaction();
//    parallel_for (uint32_t i =2 ; i< 10;i++){
//        vertex_dictionary_t::const_accessor accessor1, accessor2;
//
//        VertexDictionary->find(accessor1, 1);
//        VertexDictionary->find(accessor2, i);
//        lg::vertex_t internal_source_id = accessor1->second;
//        lg::vertex_t internal_destination_id = accessor2->second;
//        int w = 1;string_view weight { (char*) &w, sizeof(w) };
//        tx1.put_edge(internal_source_id, /* label */ 0, internal_destination_id, weight);
//    }
//    tx1.commit();

    // read edge
//    auto tx2 = G->begin_read_only_transaction();
//    parallel_for (uint32_t i =2 ; i< 10;i++){
//
//        auto tx2 = G->begin_read_only_transaction();
//        vertex_dictionary_t::const_accessor accessor1, accessor2;
//        if(VertexDictionary->find(accessor1, 1) && VertexDictionary->find(accessor2, i)){
//            lg::vertex_t internal_source_id = accessor1->second;
//            lg::vertex_t internal_destination_id = accessor2->second;
//            string_view lg_weight = tx2.get_edge(internal_source_id, /* label */ 0, internal_destination_id);
//        }
//
//    }
//    tx2.abort();
    del_G();
}


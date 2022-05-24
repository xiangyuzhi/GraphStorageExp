//
// Created by zxy on 4/25/22.
//
#include<livegraph.hpp>
#include<iostream>
#include "tbb/concurrent_hash_map.h"
using namespace std;


using vertex_dictionary_t = tbb::concurrent_hash_map<uint64_t, lg::vertex_t>;
#define VertexDictionary reinterpret_cast<vertex_dictionary_t*>(m_pHashMap)
void* m_pHashMap;
lg::vertex_t internal_id = 0;
lg::Graph* G;

void init_graph(){
    G = new lg::Graph();
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




int main(){

    init_graph();
//    add_vertex(111);
//    del_vertex(0);
    test_newv();
    cout<<"Helo"<<endl;


}


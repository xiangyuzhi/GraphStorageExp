//
// Created by zxy on 4/14/22.
//


#include<iostream>
#include "llama.h"
#include "delete_edges.h"
using namespace std;

int main(){
    srand((unsigned) time(0));

    char* database_directory = (char*) alloca(16);
    strcpy(database_directory, "db");
    int num_threads = -1;

    ll_database database(database_directory);
    if (num_threads > 0) database.set_num_threads(num_threads);
    ll_writable_graph& G = *database.graph();

    ll_t_delete_edges<ll_writable_graph> A;

    A.run();
    return 0;
}




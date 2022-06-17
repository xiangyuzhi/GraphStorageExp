//
// Created by 15743 on 2022/6/7.
//
#include "aspen_test.h"
#include "test_memory.cpp"



// -gname livejournal -core 1 -f ../../../data/ADJgraph/livejournal.adj
// -gname twitter -core 1 -f ../../../data/ADJgraph/twitter.adj
// -gname slashdot -core 1 -f ../../../data/ADJgraph/slashdot.adj
// -gname orkut -s -core 1 -f ../../../data/ADJgraph/orkut.adj
// -gname friendster -core 1 -f ../../../data/ADJgraph/friendster.adj
int main(int argc, char** argv) {
    //printf("Running data using %ld threads.\n", getWorkers());
    get_memory_usage();
    commandLine P(argc, argv);
    get_memory_usage();

    load_graph(P);
    get_memory_usage();
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}




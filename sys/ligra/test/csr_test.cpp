//
// Created by zxy on 5/11/22.
//

#include "csr_test.h"

// -f ../../../data/ADJgraph/LiveJournal.adj
int main(int argc, char** argv){

    G = new CSR(true, true);
    commandLine P(argc, argv);
    load_graph(P);
    cout<< G->get_deg(1);
}
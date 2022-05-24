//
// Created by zxy on 5/11/22.
//

#include "csr_test.h"

// -src 9 -maxiters 5 -f ../../../data/slashdot.adj
int main(int argc, char** argv){

    G = new CSR(true, true);
    commandLine P(argc, argv, "./graph_bm [-t testname -r rounds -f file");
    load_graph(P);

}
//
// Created by zxy on 5/4/22.
//

#include "PCSR.h"

int main() {
    // initialize the structure
    // How many nodes you want it to start with
    PCSR pcsr = PCSR(10);

    // add some edges
    for (int i = 0; i < 5; i++) {
        pcsr.add_edge(i, i, 1);
    }
    // update the values of some edges

    for (int i = 0; i < 5; i++) {
        pcsr.add_edge_update(i, i, 2);
    }

    // print out the graph
    pcsr.print_graph();
}
//
// Created by 15743 on 2022/6/17.
//

#define CILK 1
#include "aspen.h"
#include "alg_sys.h"

// -gname livejournal -core 1 -f ../../../data/ADJgraph/livejournal.adj -log ../../../log/aspen/alg.log
int main(int argc, char** argv) {
    srand(time(NULL));
    commandLine P(argc, argv);
    auto thd_num = P.getOptionLongValue("-core", 1);
    set_num_workers(thd_num);
    printf("Running Aspen using %ld threads.\n", thd_num);
    aspen G;
    G.load_graph(P);
    G.begin_read_graph();
    run_algorithm(P, G);
    G.end_read_graph();
    G.del();
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}

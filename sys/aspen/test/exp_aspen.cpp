//
// Created by 15743 on 2022/6/9.
//
#define CILK 1
#include "aspen.h"
#include "exp_sys.h"
//#include "alg_sys.h"

// -gname livejournal -core 1 -f ../../../data/ADJgraph/livejournal.adj -log ../../../log/aspen_vertex.log
// -gname twitter -core 1 -f ../../../data/ADJgraph/twitter.adj
// -gname slashdot -core 1 -f ../../../data/ADJgraph/slashdot.adj
// -gname orkut -s -core 1 -f ../../../data/ADJgraph/orkut.adj
// -gname friendster -core 1 -f ../../../data/ADJgraph/friendster.adj
int main(int argc, char** argv) {
    srand(time(NULL));
    commandLine P(argc, argv);
    auto thd_num = P.getOptionLongValue("-core", 1);
    set_num_workers(thd_num);
    printf("Running Aspen using %ld threads.\n", thd_num);
    aspen G;
    batch_ins_read_v(P, G);
    batch_ins_del_read_e(P, G);


    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}

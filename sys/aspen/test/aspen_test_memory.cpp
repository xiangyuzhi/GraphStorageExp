//
// Created by 15743 on 2022/6/7.
//
#include "aspen_test.h"
//#include "test_memory.cpp"
double get_memory_usage() {
    FILE*file=fopen("/proc/meminfo","r");
    if(file == NULL){
        fprintf(stderr,"cannot open /proc/meminfo\n");
        return -1;
    }
    char keyword[20];
    char valuech[20];
    double mem        =0;
    double free_mem   =0;
    fscanf(file,"MemTotal: %s kB\n",keyword);
    mem=double(atol(keyword))/(1024*1024);
    fscanf(file,"MemFree: %s kB\n",valuech);
    fscanf(file,"MemAvailable: %s kB\n",valuech);
    free_mem=double(atol(valuech))/(1024*1024);
    fclose(file);
    fprintf(stderr,"Memory %.3f GB \\ %.3f GB.\n", mem, free_mem);
    return free_mem;
}


// -gname livejournal -core 16 -f ../../../data/ADJgraph/livejournal.adj -log ../../../log/aspen/mem.log
// -gname twitter -core 16 -f ../../../data/ADJgraph/twitter.adj -log ../../../log/aspen/mem.log
// -gname orkut -core 16 -f ../../../data/ADJgraph/orkut.adj -log ../../../log/aspen/mem.log
// -gname friendster -core 16 -f ../../../data/ADJgraph/friendster.adj -log ../../../log/aspen/mem.log
// -gname graph500-24 -core 16 -f ../../../data/ADJgraph/graph500-24.adj -log ../../../log/aspen/mem.log
// -gname uniform-24 -core 16 -f ../../../data/ADJgraph/uniform-24.adj -log ../../../log/aspen/mem.log
int main(int argc, char** argv) {
    //printf("Running data using %ld threads.\n", getWorkers());
    commandLine P(argc, argv);
    double before = get_memory_usage();

    load_graph(P);
    double over = get_memory_usage();

    auto log = P.getOptionValue("-log", "none");
    auto gname = P.getOptionValue("-gname", "none");

    std::ofstream alg_file(log, ios::app);
    alg_file << gname<<","<<(before - over)<< std::endl;
    alg_file.close();
    printf("%.3f \n", before - over);
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}




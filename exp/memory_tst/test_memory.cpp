//
// Created by zxy on 5/13/22.
//

#include "memory_profiler.cpp"
#include "map"



void memory_alc(uint n){
//    uint n =1000000;
    int *newM  = reinterpret_cast<int*>(malloc(n*sizeof(int)));
    for(int i = 0;i < n;i++) newM[i] = i;
    cout<<"memory footprint: "<< gfe_compute_memory_footprint()<<" B"<<endl;
    free(newM);
    cout<<"memory footprint: "<< gfe_compute_memory_footprint()<<" B"<<endl;

}

// -t BFS -src 1 -r 4 -s -f ../../data/LiveJournal.adj
// -t BFS -src 1 -r 4 -s -f ../../data/slashdot.adj
// -t BFS -src 1 -r 4 -s -f ../../data/orkut.adj
int main(int argc, char** argv) {
    //printf("Running data using %ld threads.\n", getWorkers());
    //commandLine P(argc, argv, "./test_parallel [-t testname -r rounds -f file -m (mmap)]");

    //load_graph(P);
    memory_alc(100000);
    memory_alc(100000);
    memory_alc(100000);
    memory_alc(100000);
    cout<<"memory footprint: "<< gfe_compute_memory_footprint()<<" B"<<endl;

    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}








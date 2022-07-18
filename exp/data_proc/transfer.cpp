#include <iostream>
#include <fstream>
#include <string>

#define OPENMP 1
#include "transfer_utils/graphIO.h"
#include "transfer_utils/parseCommandLine.h"
#include "vector"
#include <sys/sysinfo.h>
#include <unistd.h>
#include <omp.h>

using namespace std;
#define newA(__E,__n) (__E*) malloc((__n)*sizeof(__E))

// 实时获取程序占用的内存，单位：kb。
size_t physical_memory_used_by_process()
{
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != nullptr) {
        if (strncmp(line, "VmRSS:", 6) == 0) {
            int len = strlen(line);

            const char* p = line;
            for (; std::isdigit(*p) == false; ++p) {}

            line[len - 3] = 0;
            result = atoi(p);

            break;
        }
    }

    fclose(file);

    return result;
}


long get_memory_usage()
{
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
    return (long)(free_mem);
}



void transfer_twitter(commandLine &P){

    char* iFile = P.getArgument(1);
    char* oFile = P.getArgument(0);
    bool sym = P.getOption("-s");

    ifstream infile;
    infile.open(iFile, ios::in);
    if (!infile.is_open()){
        cout << "Open File Failed." << endl;
    }
    string s,t;
    getline(infile,s);
    uintT NumEdge,MaxSrc,MaxDst;

    cout<<s<<endl;

    vector<edge<uintT> > Ev;
    //auto *E = newA(edge<uintT>, NumEdge);
    uintT src = 0,dst = 0;
    NumEdge = 0;
    while(infile >> src){
        infile >> dst;
        Ev.emplace_back(edge<uintT>(src , dst));
        MaxSrc = max(MaxSrc,src);
        MaxDst = max(MaxDst,dst);
        NumEdge ++;
    }
    cout<<src<<' '<<dst<<endl;
    cout<<MaxSrc<<' '<<MaxDst<<' '<<NumEdge<<' '<<Ev.size()<<endl;
    cout<<"memory "<<get_memory_usage()<<endl;

    auto *E = newA(edge<uintT>, NumEdge);
    for(uint i = 0;i<Ev.size();i++){
        E[i] = Ev[i];
    }
    cout<<"memory "<<get_memory_usage()<<endl;

    uintT maxrc = max(MaxSrc,MaxDst)+1;
    edgeArray<uintT> G = edgeArray<uintT>(E, maxrc, maxrc, NumEdge);
    cout<<"memory "<<get_memory_usage()<<endl;


    graph gg =  graphFromEdges(G,sym);
    cout<<"memory "<<get_memory_usage()<<endl;
    writeGraphToFile<uintT>(gg, oFile);
    free(E);
    Ev.clear();
}

void transfer_tsv(commandLine &P){
    char* iFile = P.getArgument(1);
    char* oFile = P.getArgument(0);
    bool sym = P.getOption("-s");

    ifstream infile;
    infile.open(iFile, ios::in);
    if (!infile.is_open()){
        cout << "Open File Failed." << endl;
    }
    string s,t;
    getline(infile,s);
    uintT NumEdge,MaxSrc,MaxDst;
    infile >>t>>NumEdge>>MaxSrc>>MaxDst;

    cout<<s<<endl<<t<<' '<<NumEdge<<' '<<MaxSrc<<' '<<MaxDst<<endl;


    auto *E = newA(edge<uintT>, NumEdge);
    uintT src,dst;
    for (int i = 0;i <NumEdge ; i++){
        infile >> src >> dst;
        //cout << src <<' ' <<dst<< endl;
        E[i] = edge<uintT>(src , dst);
    }

    cout<<" Transfer use "<<getWorkers()<<" threads."<<endl;

    uintT maxrc = max(MaxSrc,MaxDst)+1;
    edgeArray<uintT> G = edgeArray<uintT>(E, maxrc, maxrc, NumEdge);


    writeGraphToFile<uintT>(graphFromEdges(G,sym),oFile);

    cout<<" Transfer Over."<<endl;

}


void transfer_ldbc(commandLine &P){
    char* iFile = P.getArgument(1);
    char* oFile = P.getArgument(0);
    bool sym = P.getOption("-s");


    cout<<iFile<<' '<<oFile<<endl;
    ifstream infile;
    infile.open(iFile, ios::in);
    if (!infile.is_open()){
        cout << "Open File Failed." << endl;
    }
    uintT NumEdge,MaxSrc,MaxDst;

    vector<edge<uintT> > Ev;
    uintT src = 0,dst = 0;
    NumEdge = 0;
    while(infile >> src){
        infile >> dst;
        Ev.emplace_back(edge<uintT>(src , dst));
        MaxSrc = max(MaxSrc,src);
        MaxDst = max(MaxDst,dst);
        NumEdge ++;
    }
    cout<<src<<' '<<dst<<endl;
    cout<<MaxSrc<<' '<<MaxDst<<' '<<NumEdge<<' '<<Ev.size()<<endl;

    auto *E = newA(edge<uintT>, Ev.size());
    for(uint i = 0;i<Ev.size();i++){
        E[i] = Ev[i];
    }
    Ev.clear();
    uintT maxrc = max(MaxSrc,MaxDst)+1;
    edgeArray<uintT> G = edgeArray<uintT>(E, maxrc, maxrc, NumEdge);

    graph gg =  graphFromEdges(G,sym);
    writeGraphToFile<uintT>(gg,oFile);

    cout<<" Transfer Over."<<endl;
}


// ../../data/TSVgraph/out.soc-LiveJournal1 ../../data/ADJgraph/livejournal.adj
// -s ../../data/TSVgraph/out.orkut-links ../../data/ADJgraph/orkut.adj
// ../../data/TSVgraph/out.twitter ../../data/ADJgraph/twitter.adj
// ../../data/TSVgraph/out.friendster ../../data/ADJgraph/friendster.adj
// -s ../../data/LDBCgraph/graph500-24.e ../../data/ADJgraph/graph500-24.adj
// -s ../../data/LDBCgraph/uniform-24.e ../../data/ADJgraph/uniform-24.adj
// -s ../../data/TSVgraph/out.dimacs10-uk-2007-05 ../../data/ADJgraph/uk.adj

int main(int argc, char* argv[])
{
    commandLine P(argc,argv);
    transfer_ldbc(P);
//    transfer_tsv(P);
//    transfer_twitter(P);

//    int numProcs = omp_get_num_procs();
//    cout << "omp_get_num_procs() = " << numProcs << endl;

//    cout<<physical_memory_used_by_process()<<endl;
//    auto *tt = newA(int, 5000000000);
//    auto *E = newA(edge<uintT>, 5000000000);//3301876564
//    sleep(2);
//    cout<<physical_memory_used_by_process()<<endl;
//
//    free(tt);
//    auto *E2 = newA(edge<uintT>, 5000000000);
//    sleep(2);
//    cout<<physical_memory_used_by_process()<<endl;
//
//    free(E);
//    cout<<physical_memory_used_by_process()<<endl;
//
//    free(E2);
//    cout<<physical_memory_used_by_process()<<endl;
}


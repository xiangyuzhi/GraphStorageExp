//
// Created by 15743 on 2022/6/4.
//

//
// Created by 15743 on 2022/5/27.
//

#define OPENMP 1
#include "transfer_utils/graphIO.h"
#include "transfer_utils/parseCommandLine.h"
#include "vector"
using namespace std;
#define newA(__E,__n) (__E*) malloc((__n)*sizeof(__E))


void read_ldbc(commandLine &P){
    char* iFile = P.getArgument(0);
    bool sym = P.getOption("-s");

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

//    auto *E = newA(edge<uintT>, Ev.size());
//    for(uint i = 0;i<Ev.size();i++){
//        E[i] = Ev[i];
//    }
//    Ev.clear();
//    uintT maxrc = max(MaxSrc,MaxDst)+1;
//    edgeArray<uintT> G = edgeArray<uintT>(E, maxrc, maxrc, NumEdge);
//
//    graph gg =  graphFromEdges(G,0);
}


// ../../data/TSVgraph/out.soc-LiveJournal1
// ../../data/TSVgraph/out.orkut-links
// ../../data/TSVgraph/out.twitter
// ../../data/LDBCgraph/graph500-24.e
// ../../data/LDBCgraph/uniform-24.e
int main(int argc, char* argv[])
{
    commandLine P(argc,argv);
    read_ldbc(P);

}


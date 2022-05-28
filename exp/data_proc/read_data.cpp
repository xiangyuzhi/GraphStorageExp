//
// Created by 15743 on 2022/5/27.
//

#define OPENMP 1
#include "transfer_utils/graphIO.h"
#include "transfer_utils/parseCommandLine.h"

using namespace std;
#define newA(__E,__n) (__E*) malloc((__n)*sizeof(__E))


// -core 1 -f ../../data/TSVgraph/out.soc-LiveJournal1
// -core 1 -f ../../data/TSVgraph/out.orkut-links
int main(int argc, char* argv[])
{
    commandLine P(argc,argv,"[-s] <input SNAP file> <output Ligra file>");
    char* iFile = P.getArgument(0);
    bool sym = P.getOption("-s");


    ifstream infile;
    infile.open(iFile, ios::in);
    if (!infile.is_open()){
        cout << "Open File Failed." << endl;
        return 0;
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
        if(i==NumEdge-1) cout<<src<<' '<<dst<<endl;
    }

    uintT maxrc = max(MaxSrc,MaxDst)+1;
    edgeArray<uintT> G = edgeArray<uintT>(E, maxrc, maxrc, NumEdge);




}











#include <iostream>
#include <fstream>
#include <string>

#define OPENMP 1
#include "transfer_utils/graphIO.h"
#include "transfer_utils/parseCommandLine.h"
#include "vector"


using namespace std;
#define newA(__E,__n) (__E*) malloc((__n)*sizeof(__E))


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
    uintT src,dst;
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

    graph gg =  graphFromEdges(G,0);

    writeGraphToFile<uintT>(gg, oFile);

}



void transfer_other(commandLine &P){
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



// ../../data/TSVgraph/out.soc-LiveJournal1 ../../data/ADJgraph/livejournal.adj
// -s ../../data/TSVgraph/out.orkut-links ../../data/ADJgraph/orkut.adj
// ../../data/TSVgraph/out.twitter ../../data/ADJgraph/twitter.adj
// ../../data/TSVgraph/out.friendster ../../data/ADJgraph/friendster.adj
int main(int argc, char* argv[])
{
    commandLine P(argc,argv,"[-s] <input SNAP file> <output Ligra file>");
    transfer_twitter(P);

}
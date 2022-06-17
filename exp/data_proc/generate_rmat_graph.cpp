//
// Created by 15743 on 2022/6/3.
//

#include <iostream>
#include <fstream>
#include <string>

#define OPENMP 1
#include "transfer_utils/graphIO.h"
#include "transfer_utils/parseCommandLine.h"
#include "utils/rmat_util.h"
#include "vector"
typedef unsigned int uintV;

using namespace std;
#define newA(__E,__n) (__E*) malloc((__n)*sizeof(__E))


void generate_rmat(commandLine &P){
    char* outFile = P.getArgument(0);
    bool sym = P.getOption("-s");

    cout<< outFile<<endl;


    int NumEdge = P.getOptionLongValue("-enum", 80);
    int NumVertex = P.getOptionLongValue("-evertex", 60);

    auto *E = newA(edge<uintT>, NumEdge);


    double a = 0.5;
    double b = 0.1;
    double c = 0.1;
    size_t nn = 1 << (log2_up(NumVertex) - 1);
    auto r = random_aspen();
    auto rmat = rMat<uint32_t>(nn, r.ith_rand(100), a, b, c);

    uintT src,dst;
    for (uint32_t i = 0; i < NumEdge; i++) {
        std::pair<uint32_t, uint32_t> e = rmat(i);

        E[i] = edge<uintT>(e.first , e.second);
    }

    cout<<" Generate use "<<getWorkers()<<" threads."<<endl;

    uintT maxrc = NumVertex+1;
    edgeArray<uintT> G = edgeArray<uintT>(E, maxrc, maxrc, NumEdge);


    writeGraphToFile<uintT>(graphFromEdges(G,sym), outFile);

    cout<<" Generate Over."<<endl;

}



//  ../../data/ADJgraph/rmat-skewed.adj
int main(int argc, char* argv[])
{
    commandLine P(argc,argv);
    generate_rmat(P);

}
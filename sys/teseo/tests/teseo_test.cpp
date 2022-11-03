//
// Created by zxy on 4/12/22.
//

//
// Created by zxy on 4/11/22.
//
#define OPENMP 1
#include "teseo_test.h"
#include <vector>
#include <thread>
using namespace std;

int main(int argc, const char* argv[]){

    teseo::Teseo* database = new teseo::Teseo();

    auto tx1 = database->start_transaction();
    for(int i=0;i<1000000;i++)
        tx1.insert_vertex(i);
    tx1.commit();


    auto tx2 = database->start_transaction();
    for(int i=1;i<1000000;i++)
        tx2.insert_edge(i, 0, i);
    tx2.commit();

    omp_set_num_threads(16);
    gettimeofday(&t_start, &tzp);
    auto ttx = database->start_transaction(true);
    teseo::Iterator itr = ttx.iterator();
    auto &itt = itr;
    RegisterThread m_thread_tracking = RegisterThread(database);
    //teseo::Transaction& stx = *ttx;
    #pragma omp parallel for schedule(dynamic, 512) firstprivate(itt)
    for(uint64_t v = 1; v < 1000000; v++){
        itt.edges(v, false, [](uint64_t destination){});
    }
    gettimeofday(&t_end, &tzp);
    cout<<cal_time_elapsed(&t_start, &t_end)<<endl;

//    cout<<tx3.num_vertices()<<endl;
//    cout<<tx3.logical_id(10)<<endl;
//    OMP = new OpenMP(&database);
//    OpenMP &omp = *OMP;
//    uint64_t self_index = 10;
//
//    omp.iterator().edges(self_index, /* logical ? */ false, [&](uint64_t v, double wgh){
//        uint32_t w = wgh;
//        cout<<v<<' '<<w<<endl;
//    });


//    cout<<tx3.
//    auto tx4 = database.start_transaction(true);
//    auto iter = tx3.iterator();
//    iter.edges(10, /* logical ? */ false, [&](uint64_t v, double wgh){
//        printf("v : %lu  w : %.6f\n", v, wgh);
//    });
//    printf("\n");
////    iter = tx2.iterator();
//    iter.edges(20, /* logical ? */ false, [&](uint64_t v, double wgh){
//        printf("v : %lu  w : %.6f\n", v, wgh);
//    });
//    printf("\n");
////    iter = tx2.iterator();
//    iter.edges(30, /* logical ? */ false, [&](uint64_t v, double wgh){
//        printf("v : %lu  w : %.6f\n", v, wgh);
//    });
//    printf("\n");
////    iter = tx2.iterator();
//    iter.edges(40, /* logical ? */ false, [&](uint64_t v, double wgh){
//        printf("v : %lu  w : %.6f\n", v, wgh);
//    });
//    printf("\n");
//    printf("Test has been executed !!! !!! !!!\n");
    return 0;
}

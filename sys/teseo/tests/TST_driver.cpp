//
// Created by 15743 on 2022/7/15.
//

#define OPENMP 1
#include "teseo_test.h"
#include "teseo_openmp.hpp"
#include <vector>
#include <thread>
using namespace std;


static
unique_ptr<double[]> teseo_pagerank(OpenMP& openmp, uint64_t num_iterations, double damping_factor/*, utility::TimeoutService& timer*/) {
    // init
    const uint64_t num_vertices = openmp.transaction().num_vertices();
    //COUT_DEBUG("num vertices: " << num_vertices);

    const double init_score = 1.0 / num_vertices;
    const double base_score = (1.0 - damping_factor) / num_vertices;

    unique_ptr<double[]> ptr_scores{ new double[num_vertices]() }; // avoid memory leaks
    double* scores = ptr_scores.get();
#pragma omp parallel for
    for(uint64_t v = 0; v < num_vertices; v++){
        scores[v] = init_score;
    }
    vector<double> outgoing_contrib(num_vertices, 0.0);

    // pagerank iterations
    for(uint64_t iteration = 0; iteration < num_iterations /*&& !timer.is_timeout()*/; iteration++){
        double dangling_sum = 0.0;

        // for each node, precompute its contribution to all of its outgoing neighbours and, if it's a sink,
        // add its rank to the `dangling sum' (to be added to all nodes).
#pragma omp parallel for reduction(+:dangling_sum) firstprivate(openmp)
        for(uint64_t v = 0; v < num_vertices; v++){
            uint64_t out_degree = openmp.transaction().degree(v, /* logical */ true);
            if(out_degree == 0){ // this is a sink
                dangling_sum += scores[v];
            } else {
                outgoing_contrib[v] = scores[v] / out_degree;
            }
        }

        dangling_sum /= num_vertices;

        // compute the new score for each node in the graph
#pragma omp parallel for schedule(dynamic, 64) firstprivate(openmp)
        for(uint64_t v = 0; v < num_vertices; v++){

            double incoming_total = 0;
            openmp.iterator().edges(v, /* logical ? */ true, [&incoming_total, &outgoing_contrib](uint64_t destination){
                incoming_total += outgoing_contrib[destination];
            });

            // update the score
            scores[v] = base_score + damping_factor * (incoming_total + dangling_sum);
        }
    }

    return ptr_scores;
}

//void pagerank(uint64_t num_iterations, double damping_factor, const char* dump2file) {
//OpenMP openmp( G ); // OpenMP machinery
//
//// Init
////utility::TimeoutService timeout { m_timeout };
////Timer timer; timer.start();
//
//// Run the PageRank algorithm
//unique_ptr<double[]> ptr_rank = teseo_pagerank(openmp, num_iterations, damping_factor, timeout);
//if(timeout.is_timeout()){ RAISE_EXCEPTION(TimeoutError, "Timeout occurred after " << timer);  }
//
//// translate the vertex IDs
//const uint64_t N = openmp.transaction().num_vertices();
//auto external_ids = translate(openmp, ptr_rank.get(), N);
//if(timeout.is_timeout()){ RAISE_EXCEPTION(TimeoutError, "Timeout occurred after " << timer); }
//
//// store the results in the given file
//if(dump2file != nullptr)
//save_results(external_ids, dump2file);
//}

int main(int argc, char** argv){

    teseo::Teseo database;


    omp_set_num_threads(16);

    auto tx1 = database.start_transaction();

    tx1.insert_vertex(1);
    tx1.commit();

    database.register_thread();
    auto txx = database.start_transaction(true);
    printf("n: %d\n", txx.num_vertices());
    database.unregister_thread();

    parallel_for(int i = 2;i <=20;i+=1){
        database.register_thread();
        auto tx = database.start_transaction();
        tx.insert_vertex(i);
        tx.insert_edge(i, 1, /* weight */ i);
        tx.commit();
        printf("%d \n", omp_get_thread_num());
        database.unregister_thread();
    }
    cout<<"insert over"<<endl;

    parallel_for(int i = 2;i <=20;i+=1){
        database.register_thread();
        auto tx2 = database.start_transaction(true);
        auto iter = tx2.iterator();
        iter.edges(i, /* logical ? */ false, [&](uint64_t v, double wgh){
            printf("u: %u, v : %u, w : %.6f from %d\n", i, v, wgh, omp_get_thread_num());
        });
        database.unregister_thread();
    }

    database.register_thread();
    auto tx = database.start_transaction(true);
    printf("n: %d\n", tx.num_vertices());
    database.unregister_thread();

    commandLine P(argc, argv );
    auto thd_num = P.getOptionLongValue("-core", 1);
    omp_set_num_threads(thd_num);
    printf("Running Teseo using %ld threads.\n", thd_num );

    load_graph(P);
    OpenMP openmp( G );
    std::vector<double> total_time;
    for (size_t i=0; i<4; i++) {
        gettimeofday(&t_start, &tzp);
        unique_ptr<double[]> ptr_rank = teseo_pagerank(openmp, 10, 0.8);
        gettimeofday(&t_end, &tzp);
        double tm = cal_time_elapsed(&t_start, &t_end);
        std::cout << "\ttest=" << "pagerank" << "\ttime=" << tm << "\titeration=" << i << std::endl;
        total_time.emplace_back(tm);
    }
    double avg_time = cal_time(total_time);
    std::cout << "\ttest=" << "pagerank"<< "\ttime=" << avg_time << "\tgraph=" << "livejournal" << std::endl;
    return 0;
}

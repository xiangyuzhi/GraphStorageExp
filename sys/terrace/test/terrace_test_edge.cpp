//
// Created by zxy on 4/26/22.
//
#define OPENMP 1
#include "terrace_test.h"

void load_graph(commandLine& P){
    auto filename = P.getOptionValue("-f", "none");
    pair_uint *edges = get_edges_from_file_adj_sym(filename.c_str(), &num_edges, &num_nodes);
    G = new Graph(num_nodes);
    for (uint32_t i = 0; i < num_edges; i++) {
        new_srcs.push_back(edges[i].x);
        new_dests.push_back(edges[i].y);
    }
    auto perm = get_random_permutation(num_edges);

    PRINT("=============== Load Graph BEGIN ===============");
    gettimeofday(&t_start, &tzp);
    G->add_edge_batch(new_srcs.data(), new_dests.data(), num_edges, perm);
    gettimeofday(&t_end, &tzp);
    free(edges);
    new_srcs.clear();
    new_dests.clear();
    float size_gb = G->get_size() / (float) 1073741824;
    PRINT("Load Graph: Nodes: " << G->get_num_vertices() <<" Edges: " << G->get_num_edges() << " Size: " << size_gb << " GB");
    print_time_elapsed("Load Graph Cost: ", &t_start, &t_end);
    PRINT("Throughput: " << G->get_num_edges() / (float) cal_time_elapsed(&t_start, &t_end));
    PRINT("================ Load Graph END ================");
}


void batch_ins_del_read(commandLine& P){
    PRINT("=============== Batch Insert BEGIN ===============");

    auto gname = P.getOptionValue("-gname", "none");
    std::ofstream alg_file("../../../log/terrace/edge.log",ios::app);
    alg_file << "GRAPH" << "\t"+gname <<"\t["<<getCurrentTime0()<<']'<<std::endl;
    auto thd_num = P.getOptionLongValue("-core", 1);
    alg_file << "Using threads :" << "\t"<<thd_num<<endl;

    Graph Ga = *G;
    std::vector<uint32_t> update_sizes = {10000000};//10, 100, 1000 ,10000,100000,1000000,
    auto r = random_aspen();
    auto update_times = std::vector<double>();
    size_t n_trials = 1;
    for (size_t us=0; us<update_sizes.size(); us++) {
        double avg_insert = 0;
        double avg_delete = 0;
        double avg_read = 0;
        std::cout << "Running batch size: " << update_sizes[us] << std::endl;

        if (update_sizes[us] < 10000000)
            n_trials = 20;
        else n_trials = 5;
        size_t updates_to_run = update_sizes[us];
        auto perm = get_random_permutation(updates_to_run);
        for (size_t ts=0; ts<n_trials; ts++) {
            uint32_t num_nodes = Ga.get_num_vertices();

            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            size_t nn = 1 << (log2_up(num_nodes) - 1);
            auto rmat = rMat<uint32_t>(nn, r.ith_rand(100+ts), a, b, c);
            parallel_for(uint32_t i = 0; i < updates_to_run; i++) {
                std::pair<uint32_t, uint32_t> edge = rmat(i);
                new_srcs.push_back(edge.first);
                new_dests.push_back(edge.second);
            }
            pair_uint *edges = (pair_uint*)calloc(updates_to_run, sizeof(pair_uint));
            for (uint32_t i = 0; i < updates_to_run; i++) {
                edges[i].x = new_srcs[i];
                edges[i].y = new_dests[i];
            }
            integerSort_y((pair_els*)edges, updates_to_run, num_nodes);
            integerSort_x((pair_els*)edges, updates_to_run, num_nodes);
            new_srcs.clear();
            new_srcs.clear();
            for (uint32_t i = 0; i < updates_to_run; i++) {
                new_srcs.push_back(edges[i].x);
                new_dests.push_back(edges[i].y);
            }

            gettimeofday(&t_start, &tzp);
            Ga.add_edge_batch(new_srcs.data(), new_dests.data(), updates_to_run, perm);
            gettimeofday(&t_end, &tzp);
            avg_insert += cal_time_elapsed(&t_start, &t_end);


            gettimeofday(&t_start, &tzp);
            for(uint32_t i = 0; i < updates_to_run; i++) {
                if (!Ga.is_edge(new_srcs[i], new_dests[i])) {
                    printf("edge (%u, %u) not found, should be\n", new_srcs[i], new_dests[i]);
                    printf("\tdegree %u = %u\n", new_srcs[i], Ga.degree(new_srcs[i]));
                    return;
                }
            }
            gettimeofday(&t_end, &tzp);
            avg_read += cal_time_elapsed(&t_start, &t_end);

            gettimeofday(&t_start, &tzp);
            parallel_for(uint32_t i = 0; i < updates_to_run; i++) {
                Ga.remove_edge(new_srcs[i], new_dests[i]);
            }
            gettimeofday(&t_end, &tzp);
            avg_delete +=  cal_time_elapsed(&t_start, &t_end);
        }
        double time_i = (double) avg_insert / n_trials;
        double insert_throughput = updates_to_run / time_i;
        printf("batch_size = %zu, average insert: %f, throughput %e\n", updates_to_run, time_i, insert_throughput);
        alg_file <<"\t["<<getCurrentTime0()<<']' << "AVG"<< "\tbatch_size=" << updates_to_run<< "\ttime=" << time_i<< "\tthroughput=" << insert_throughput << std::endl;

        double time_r = (double) avg_read / n_trials;
        double read_throughput = updates_to_run / time_r;
        printf("batch_size = %zu, average read: %f, throughput %e\n", updates_to_run, time_r, read_throughput);
        alg_file <<"\t["<<getCurrentTime0()<<']' << "AVG"<< "\tbatch_size=" << updates_to_run<< "\ttime=" << time_r<< "\tthroughput=" << read_throughput << std::endl;

        double time_d = (double) avg_delete / n_trials;
        double delete_throughput = updates_to_run / time_d;
        printf("batch_size = %zu, average delete: %f, throughput %e\n", updates_to_run, time_d, delete_throughput);
        alg_file <<"\t["<<getCurrentTime0()<<']' << "AVG"<< "\tbatch_size=" << updates_to_run<< "\ttime=" << time_d<< "\tthroughput=" << delete_throughput << std::endl;
    }
    PRINT("=============== Batch Insert END ===============");
}


// -src 9 -maxiters 5 -f ../../../data/slashdot.adj
// -src 9 -maxiters 5 -f ../../../data/orkut.adj

int main(int argc, char** argv) {
    srand(time(NULL));

    commandLine P(argc, argv, "./graph_bm [-t testname -r rounds -f file");
    auto thd_num = P.getOptionLongValue("-core", 16);
    omp_set_num_threads(thd_num);
    printf("Running Terrace using %ld threads.\n", thd_num );

    load_graph(P);

    batch_ins_del_read(P);

    del_graph();
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}
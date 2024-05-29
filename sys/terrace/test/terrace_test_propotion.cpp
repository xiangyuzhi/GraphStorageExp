#define CILK 1
// #define OPENMP 1
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
    auto thd_num = P.getOptionLongValue("-core", 1);
    auto log = P.getOptionValue("-log","none");
    std::ofstream log_file(log, ios::app);

    // std::vector<uint32_t> update_sizes = {10, 100, 1000, 10000, 100000, 1000000, 10000000};
    std::vector<uint32_t> update_sizes = {900000};
    std::vector<uint32_t> update_sizes2 = {100000};
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
        size_t updates_to_run2 = update_sizes2[us];
        auto perm = get_random_permutation(updates_to_run);
        for (size_t ts=0; ts<n_trials; ts++) {
            uint32_t num_nodes = G->get_num_vertices();

            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            size_t nn = 1 << (log2_up(num_nodes) - 1);
            auto rmat = rMat<uint32_t>(nn, r.ith_rand(100+ts), a, b, c);
            for(uint32_t i = 0; i < updates_to_run; i++) {
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
            query_srcs.clear();
            query_dests.clear();

            for (uint32_t i = 0; i < updates_to_run; i++) {
                new_srcs.push_back(edges[i].x);
                new_dests.push_back(edges[i].y);
            }
            free(edges);

            // generate random deges from new_srcs and new_dests
            std::default_random_engine generator;
            std::uniform_int_distribution<size_t> distribution(0, new_srcs.size() - 1);
            for (size_t i = 0; i < updates_to_run2; i++) {
                size_t index = distribution(generator);
                query_srcs.push_back(new_srcs[index]);
                query_dests.push_back(new_dests[index]);
            }

            gettimeofday(&t_start, &tzp);

            // G->add_edge_batch(new_srcs.data(), new_dests.data(), updates_to_run, perm);
            // for(uint32_t i = 0; i < updates_to_run2; i++) {
            //     G->is_edge(query_srcs[i], query_dests[i]);
            // }

            uint32_t smaller_updates = updates_to_run < updates_to_run2 ? updates_to_run : updates_to_run2;

            // for(uint32_t round = 0; round < smaller_updates; round++){
            //     G->add_edge(new_srcs[round], new_dests[round]);
            //     G->is_edge(query_srcs[round], query_dests[round]);          
            // }
            
            // for(uint32_t round = 0; round < smaller_updates; round++){
            //     G->add_edge(new_srcs[round], new_dests[round]);
            //     for(uint32_t p = 0;p < 9; p++) {
            //         G->is_edge(query_srcs[round*9+p], query_dests[round*9+p]);
            //     }
            // }

            for(uint32_t round = 0; round < smaller_updates; round++){
                for(uint32_t p = 0;p < 9; p++) {
                    G->add_edge(new_srcs[round*9+p], new_dests[round*9+p]);
                }
                G->is_edge(query_srcs[round], query_dests[round]);
            }

            

            gettimeofday(&t_end, &tzp);
            avg_insert += cal_time_elapsed(&t_start, &t_end);

            // remove edges 
            // puts("debug-----remove edges");
            for(uint32_t i = 0; i < updates_to_run; i++) {
                G->remove_edge(new_srcs[i], new_dests[i]);
            }
        }
        double time_i = (double) avg_insert / n_trials;
        double insert_throughput = (updates_to_run+updates_to_run2) / time_i;
        printf("batch_size = %zu, average insert: %f, throughput %e\n", updates_to_run, time_i, insert_throughput);
        log_file<< gname<<","<<thd_num<<",e,insert-read,"<< update_sizes[us] <<"-" << update_sizes2[us]<<","<<insert_throughput << "\n";
    }
    PRINT("=============== Batch Insert END ===============");
}


// -gname livejournal -core 16 -f ../../../data/ADJgraph/livejournal.adj -log ../../../log/terrace/edge.log

int main(int argc, char** argv) {
    srand(time(NULL));

    commandLine P(argc, argv);
    auto thd_num = P.getOptionLongValue("-core", 1);
    set_num_workers(thd_num);
    printf("Running Terrace using %ld threads.\n", thd_num );

    load_graph(P);

    batch_ins_del_read(P);

    del_graph();
    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}
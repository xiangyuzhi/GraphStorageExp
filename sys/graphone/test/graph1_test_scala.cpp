#define  OPENMP 1
#include "graph1_test.h"
#include "omp.h"
#include "util/parallel.h"
#include "BFS.h"
#include "PR.h"
#include "TC.h"

void graph1_remove_e(uint64_t src, uint64_t dst){
    common::SpinLock& mutex = m_edge_locks[(src + dst) % m_num_edge_locks].m_lock;
    scoped_lock<common::SpinLock> xlock(mutex);
    do_update(/* is insert ? */ false, src, dst, 1.0);
}


double test_bfs(commandLine& P) {
    uint32_t bfs_src = 9;
    bool thread = P.getOption("-thread");
    if(!thread) {
        auto* view = create_static_view(get_graphone_graph(), SIMPLE_MASK | PRIVATE_MASK);
        size_t n = view->v_count;
        size_t maxdeg = 0;
        size_t maxid = 9;
        for(uint32_t i=1;i<n;i++){
            size_t src_degree = view->get_degree_out(i);
            if(src_degree > maxdeg) {
                maxdeg = src_degree;
                maxid = i;
            }
        }
        bfs_src = maxid;
    }
    std::cout << "Running BFS from source = " << bfs_src << std::endl;
    gettimeofday(&t_start, &tzp);
    auto* view2 = create_static_view(get_graphone_graph(), SIMPLE_MASK | PRIVATE_MASK);
    BFS(view2, bfs_src);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}


double test_pr(commandLine& P) {
    long maxiters = P.getOptionLongValue("-maxiters",10);
    gettimeofday(&t_start, &tzp);
    auto* view = create_static_view(get_graphone_graph(), SIMPLE_MASK | PRIVATE_MASK);
    PR<double>(view, maxiters);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double test_k_hop(commandLine& P, int k) {
    gettimeofday(&t_start, &tzp);
    auto* view = create_static_view(get_graphone_graph(), SIMPLE_MASK | PRIVATE_MASK);
    //K_HOP(view, k);
    if(k==1){
        uint32_t n = view->v_count;
        uint32_t nsrc = n/20;
        srand(n);
        parallel_for(int i=0;i<nsrc;i++){
            auto src = rand()%n;
            uint64_t degree = view->get_degree_out(src);
            lite_edge_t* neighbours = nullptr;
            neighbours = (lite_edge_t*) realloc(neighbours, sizeof(neighbours[0]) * degree);
            degree = view->get_nebrs_out(src, neighbours);
            for(uint32_t local_idx = 0; local_idx < degree; local_idx++){
                uint64_t v = get_sid(neighbours[local_idx]);
                uint32_t w = neighbours[local_idx].second.value32b;
            }
            free(neighbours); neighbours = nullptr;
        }
    }
    else {
        uint32_t n = view->v_count;
        uint32_t nsrc = n/20;
        srand(n);
        parallel_for(int i=0;i<nsrc;i++){
            auto src = rand()%n;
            uint64_t degree = view->get_degree_out(src);
            lite_edge_t* neighbours = nullptr;
            neighbours = (lite_edge_t*) realloc(neighbours, sizeof(neighbours[0]) * degree);
            degree = view->get_nebrs_out(src, neighbours);
            for(uint32_t id1 = 0; id1 < degree; id1++){
                uint64_t v = get_sid(neighbours[id1]);
                uint32_t w = neighbours[id1].second.value32b;
                uint64_t deg2 = view->get_degree_out(v);
                lite_edge_t* ngh2 = nullptr;
                ngh2 = (lite_edge_t*) realloc(ngh2, sizeof(ngh2[0]) * deg2);
                deg2 = view->get_nebrs_out(v, ngh2);
                for(uint32_t id2 = 0; id2 < deg2; id2++) {
                    uint64_t v = get_sid(ngh2[id2]);
                }
                free(ngh2); ngh2 = nullptr;
            }
            free(neighbours); neighbours = nullptr;
        }
    }

    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double test_read(commandLine& P) {
    auto* view = create_static_view(get_graphone_graph(), SIMPLE_MASK | PRIVATE_MASK);
    auto r = random_aspen();
    size_t n = view->v_count;
    double a = 0.5;
    double b = 0.1;
    double c = 0.1;
    size_t nn = 1 << (log2_up(n) - 1);
    auto rmat = rMat<uint32_t>(nn, r.ith_rand(100), a, b, c);
    new_srcs.clear();new_dests.clear();
    uint32_t updates = num_edges/20;
    for( uint32_t i = 0; i < updates; i++) {
        std::pair<uint32_t, uint32_t> edge = rmat(i);
        new_srcs.push_back(edge.first);
        new_dests.push_back(edge.second);
    }
    gettimeofday(&t_start, &tzp);
    parallel_for (uint32_t i =0 ; i< updates;i++){
        uint64_t degree = view->get_degree_out(new_srcs[i]);

        lite_edge_t* neighbours = nullptr;
        neighbours = (lite_edge_t*) realloc(neighbours, sizeof(neighbours[0]) * degree);
        degree = view->get_nebrs_out(new_srcs[i], neighbours);

        for(uint32_t local_idx = 0; local_idx < degree; local_idx++){
            uint64_t v = get_sid(neighbours[local_idx]);
            if(v == new_dests[i]) break;
        }
        free(neighbours); neighbours = nullptr;
    }

    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}

double test_tc(commandLine& P) {

    gettimeofday(&t_start, &tzp);
    auto* view = create_static_view(get_graphone_graph(), SIMPLE_MASK | PRIVATE_MASK);
    auto count = TC(view);
    gettimeofday(&t_end, &tzp);
    return cal_time_elapsed(&t_start, &t_end);
}


double execute(commandLine& P, string testname) {
    if (testname == "BFS") {
        return test_bfs(P);
    } else if (testname == "PR") {
        return test_pr(P);
    } else if (testname == "1-HOP") {
        return test_k_hop(P, 1);
    }else if (testname == "2-HOP") {
        return test_k_hop(P, 2);
    } else if (testname == "Read") {
        return test_read(P);
    } else if (testname == "TC") {
        return test_tc(P);
    } else {
        std::cout << "Unknown test: " << testname << ". Quitting." << std::endl;
        exit(0);
    }
}

void run_algorithm(commandLine& P, int thd_num, string gname) {
    PRINT("=============== Run Algorithm BEGIN ===============");

    // Run the algorithm on it
    size_t rounds = P.getOptionLongValue("-rounds", 5);
    auto log = P.getOptionValue("-log", "none");
    std::ofstream alg_file(log, ios::app);

    std::vector<std::string> test_ids;
    test_ids = {"BFS","PR","1-HOP","2-HOP","Read"};

    for (auto test_id : test_ids) {
        std::vector<double> total_time;
        for (size_t i = 0; i < rounds; i++) {
            double tm = execute(P, test_id);

            std::cout << "\ttest=" << test_id << "\ttime=" << tm << "\titeration=" << i << std::endl;
            total_time.emplace_back(tm);
        }
        double avg_time = cal_time(total_time);
        std::cout << "\ttest=" << test_id<< "\ttime=" << avg_time << "\tgraph=" << gname << std::endl;
        alg_file << gname<<","<< thd_num<<","<<test_id<<","<< avg_time << std::endl;
    }
    PRINT("=============== Run Algorithm END ===============");
}

void batch_ins_del_read(commandLine& P, int thd_num, string gname){
    PRINT("=============== Batch Insert BEGIN ===============");

    auto log = P.getOptionValue("-log","none");
    std::ofstream log_file(log, ios::app);

    std::vector<uint32_t> update_sizes = {100000};
    auto r = random_aspen();
    auto update_times = std::vector<double>();
    size_t n_trials = 1;
    for (size_t us=0; us<update_sizes.size(); us++) {
        PRINT("v size: "<<num_vertices() <<" e size: "<<m_num_edges);
        double avg_insert = 0;
        double avg_delete = 0;
        double avg_read = 0;
        std::cout << "Running batch size: " << update_sizes[us] << std::endl;

        size_t updates_to_run = update_sizes[us];
        auto perm = get_random_permutation(updates_to_run);
        if (update_sizes[us] < 10000000)
            n_trials = 20;
        else n_trials = 5;
        for (size_t ts=0; ts<n_trials; ts++) {
            uint64_t num_nodes = num_vertices();
            new_srcs.clear();
            new_dests.clear();

            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            size_t nn = 1 << (log2_up(num_nodes) - 1);
            auto rmat = rMat<uint32_t>(nn, r.ith_rand(100+ts), a, b, c);
            for( uint32_t i = 0; i < updates_to_run; i++) {
                std::pair<uint32_t, uint32_t> edge = rmat(i);
                new_srcs.push_back(edge.first);
                new_dests.push_back(edge.second);
            }

            gettimeofday(&t_start, &tzp);
            parallel_for (uint32_t i =0 ; i< updates_to_run;i++){
                graph1_add_e(new_srcs[i],new_dests[i]);
            }
            gettimeofday(&t_end, &tzp);
            avg_insert += cal_time_elapsed(&t_start, &t_end);

            gettimeofday(&t_start, &tzp);
            parallel_for(uint32_t i = 0; i < updates_to_run; i++) {
                graph1_remove_e(new_srcs[i], new_dests[i]);
            }
            gettimeofday(&t_end, &tzp);
            avg_delete +=  cal_time_elapsed(&t_start, &t_end);
        }
        double time_i = (double) avg_insert / n_trials;
        double insert_throughput = updates_to_run / time_i;
        printf("batch_size = %zu, average insert: %f, throughput %e\n", updates_to_run, time_i, insert_throughput);
        log_file<< gname<<","<<thd_num<<",e,insert,"<< update_sizes[us] <<","<<insert_throughput << "\n";

        double time_d = (double) avg_delete / n_trials;
        double delete_throughput = updates_to_run / time_d;
        printf("batch_size = %zu, average delete: %f, throughput %e\n", updates_to_run, time_d, delete_throughput);
        log_file<< gname<<"," <<thd_num<<",e,delete,"<< update_sizes[us] <<","<<delete_throughput << "\n";
    }
    PRINT("=============== Batch Insert END ===============");
}


// -gname livejournal -core 16 -f ../../../data/ADJgraph/livejournal.adj -log ../../../log/graphone/edge.log
int main(int argc, char** argv) {
    srand(time(NULL));
    commandLine P(argc, argv);
    bool thread = P.getOption("-thread");
    if(thread){
        {
            auto gname = P.getOptionValue("-gname", "none");
            load_graph(P);
            std::vector<uint32_t> threads = {1,4,8,12,16};
            for(auto thd_num : threads){
                set_num_workers(thd_num);
                cout << "Running GraphOne using " << thd_num << " threads." << endl;
                run_algorithm(P, thd_num, gname);
            }
            for(auto thd_num: threads){
                set_num_workers(thd_num);
                cout << "Running GraphOne using " << thd_num << " threads." << endl;
                batch_ins_del_read(P, thd_num, gname);
            }
            del_g();
        }
    }

    else {

        auto insert_f = [&](uint32_t deg, uint32_t logv){
            g = new class graph();
            graph1_init((1L<<logv));
            for(uint64_t i=0;i<(1L<<logv);i++)
                graph1_add_v(i);
            uint32_t Enum = (1L<<logv)*deg;
            num_edges = Enum;
            num_nodes =  (1L<<logv);
            cout<<"v: "<<logv<<" e: "<<deg<<endl;
            double a = 0.5;
            double b = 0.1;
            double c = 0.1;
            auto r = random_aspen();
            size_t nn = 1 << (log2_up(1L<<logv) - 1);
            auto rmat = rMat<uint32_t>(nn, r.ith_rand(100), a, b, c);
            for( uint32_t i = 0; i < Enum; i++) {
                std::pair<uint32_t, uint32_t> edge = rmat(i);
                graph1_add_e(edge.first,edge.second);
            }
            string gname = "graph_"+to_string(logv)+"_"+ to_string(deg);
            run_algorithm(P, 16, gname);
            batch_ins_del_read(P, 16, gname);
            del_g();
        };


        {
            auto v = P.getOptionIntValue("-v", -1);
            auto e = P.getOptionIntValue("-e", -1);
            insert_f(e, v);
        }

    }

    printf("!!!!! TEST OVER !!!!!\n");
    return 0;
}
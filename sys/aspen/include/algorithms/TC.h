//
// Created by 15743 on 2022/5/26.
//

#ifndef EXP_TC_H
#define EXP_TC_H


template<typename Graph>
uint64_t countCommon(Graph &G, uint32_t a, uint32_t b) {
    uint32_t deg_A = G.find_vertex(a).value.degree();
    auto* ngh_A = G.find_vertex(a).value.get_edges(a);
    uint32_t itr_A = 0;
    uint32_t deg_B = G.find_vertex(b).value.degree();
    auto* ngh_B = G.find_vertex(b).value.get_edges(b);
    uint32_t itr_B = 0;

    uint64_t ans=0;
    while (itr_A<deg_A && itr_B<deg_B && ngh_A[itr_A] < a && ngh_B[itr_B] < b) { //count "directed" triangles
        if (ngh_A[itr_A] == ngh_B[itr_B]) itr_A++, itr_B++, ans++;
        else if (ngh_A[itr_A] < ngh_B[itr_B]) itr_A++;
        else itr_B++;
    }
    pbbs::free_array(ngh_A);
    pbbs::free_array(ngh_B);
    return ans;
}

template<typename Graph>
struct countF { //for edgeMap
    Graph &G;
    std::vector<uint64_t> &counts;
    countF(Graph &G_, std::vector<uint64_t> &_counts) : G(G_), counts(_counts) {}
    inline bool update (uint32_t s, uint32_t d) {
        if(s > d) {//only count "directed" triangles
            counts[8*num_workers()] += countCommon(G,s,d);
        }
        return 1;
    }
    inline bool updateAtomic (uint32_t s, uint32_t d) {
        if (s > d) { //only count "directed" triangles
            counts[8*num_workers()] += countCommon(G,s,d);
        }
        return 1;
    }
    inline bool cond ([[maybe_unused]] uint32_t d) { return true; } //does nothing
};

template<typename Graph>
uint64_t TC(Graph &G) {
    size_t n = G.num_vertices();
    std::vector<uint64_t> counts(num_workers()*8, 0);
    bool* tt = pbbs::new_array_no_init<bool>(1);
    vertex_subset Frontier = vertex_subset(n,n,tt); //frontier contains all vertices

    timer sparse_t, dense_t, fetch_t, other_t;
    G.edge_map(Frontier, countF<Graph>(G, counts), sparse_t, dense_t, fetch_t, other_t, stay_dense);
    Frontier.del();
    uint64_t count = 0;
    for (int i = 0; i < num_workers(); i++) {
        count += counts[i*8];
    }
    return count;

}


#endif //EXP_TC_H

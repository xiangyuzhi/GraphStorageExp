//
// Created by 15743 on 2022/5/26.
//

#ifndef EXP_TC_H
#define EXP_TC_H

template<typename Graph>
uint64_t countCommon(Graph &G, uint32_t a, uint32_t b) {

    size_t deg_A = G.find_vertex(a).value.degree();
    auto *a_ngh = G.find_vertex(a).value.get_edges(a);
    size_t deg_B = G.find_vertex(b).value.degree();
    auto *b_ngh = G.find_vertex(b).value.get_edges(b);

    uint64_t ans=0;
    uint32_t it_A = 0, it_B = 0;
    while (it_A!=deg_A && it_B!=deg_B && a_ngh[it_A] < a && b_ngh[it_B] < b) { //count "directed" triangles
        if (a_ngh[it_A] == b_ngh[it_B]) ++it_A, ++it_B, ans++;
        else if (a_ngh[it_A] < b_ngh[it_B]) ++it_A;
        else ++it_B;
    }
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

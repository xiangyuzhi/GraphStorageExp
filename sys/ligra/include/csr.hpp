/**
 * Copyright (C) 2019 Dean De Leo, email: dleo[at]cwi.nl
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <atomic>
#include <chrono>
#include <cinttypes>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

class CSR {
protected:
    const bool m_is_directed; // whether the graph is directed
    uint64_t m_num_vertices; // total number of vertices
    uint64_t m_num_edges; // total number of edges
//    std::unordered_map<uint64_t, uint64_t> m_ext2log; // dictionary external vertex id -> logical vertex id
//    uint64_t* m_log2ext {nullptr}; // dictionary logical vertex id -> external vertex id
    uint64_t* m_out_v {nullptr}; // vertex array for the outgoing edges
    uint64_t* m_out_e {nullptr}; // edge array for the outgoing edges
    double* m_out_w {nullptr}; // weights associated to the outgoing edges
    uint64_t* m_in_v {nullptr}; // vertex array for the incoming edges (only in directed graphs)
    uint64_t* m_in_e {nullptr}; // edge array for the incoming edges (only in directed graphs)
    double* m_in_w {nullptr}; // weights associated to the incoming edges
    uint64_t m_timeout = 0; // max time to complete a kernel of the graphalytics suite, in seconds
    const bool m_numa_interleaved; // whether to use libnuma to allocate the internal arrays

    // Retrieve the [start, end) interval for the outgoing edges associated to the given logical vertex
    std::pair<uint64_t, uint64_t> get_out_interval(uint64_t logical_vertex_id) const;

    // Retrieve the [start, end) interval for the incoming edges associated to the given logical vertex
    std::pair<uint64_t, uint64_t> get_in_interval(uint64_t logical_vertex_id) const;

    // Retrieve the [start, end) interval for the edges associated to the given logical vertex
    std::pair<uint64_t, uint64_t> get_interval_impl(const uint64_t* __restrict vertex_array, uint64_t logical_vertex_id) const;

    // Retrieve the number of outgoing edges for the given vertex
    uint64_t get_out_degree(uint64_t logical_vertex_id) const;

    // Retrieve the number of incoming edges for the given vertex
    uint64_t get_in_degree(uint64_t logical_vertex_id) const;

    template<typename T>
    T* alloca_array(uint64_t sz);

    template<typename T>
    void free_array(T* array);

public:
    CSR(bool is_directed, bool numa_interleaved = false);


    ~CSR();

    uint64_t num_edges() const;

    uint64_t num_vertices() const;

    bool has_vertex(uint64_t vertex_id) const;

    double get_weight(uint64_t source, uint64_t destination) const;

    bool is_directed() const;

    void load(std::vector<std::pair<uint32_t ,std::pair<uint32_t,double> > >& edge, uint32_t N_vertex, uint64_t M_edge);

    std::pair<uint64_t, uint64_t> traverse(uint64_t logical_vertex_id) ;
    uint64_t get_deg(uint64_t logical_vertex_id) ;

    void set_timeout(uint64_t seconds);

    uint64_t get_random_vertex_id() const;


    uint64_t* out_v() const; // outgoing edges, vertex array of size |V|
    uint64_t* out_e() const; // outgoing edges, edge array of size |E|
    double* out_w() const; // outgoing edges, weight array of size |E|
    uint64_t* in_v() const; // incoming edges (only directed graphs), vertex array
    uint64_t* in_e() const; // incoming edges (only directed graphs), edge array
    double* in_w() const; // incoming edges (only directed graphs), weight array

};

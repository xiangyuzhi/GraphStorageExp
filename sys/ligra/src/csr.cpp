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

#include "csr.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <mutex>
//#include <numa.h>

#include <omp.h>
#include <random>
#include <sstream>
#include <string>
#include <unordered_set>


using namespace std;


CSR::CSR(bool is_directed, bool numa_interleaved) :
m_is_directed(is_directed), m_num_vertices (0), m_num_edges(0), m_numa_interleaved(numa_interleaved) {}

CSR::~CSR(){
    free_array(m_out_v); m_out_v = nullptr;
    free_array(m_out_e); m_out_e = nullptr;
    free_array(m_out_w); m_out_w = nullptr;

    if(m_is_directed){ // otherwise, they are simply aliases to m_out_x
        free_array(m_in_v);
        free_array(m_in_e);
        free_array(m_in_w);
    }

    m_in_v = nullptr;
    m_in_e = nullptr;
    m_in_w = nullptr;

    free_array(m_log2ext); m_log2ext = nullptr;
}

template<typename T>
T* CSR::alloca_array(uint64_t array_sz){
    return new T[array_sz]();
}

template<typename T>
void CSR::free_array(T* array){
    if(array == nullptr) return; // nop

    delete[] array;
}

uint64_t CSR::num_edges() const {
    return m_num_edges;
}

uint64_t CSR::num_vertices() const {
    return m_num_vertices;
}

bool CSR::is_directed() const {
    return m_is_directed;
}

bool CSR::has_vertex(uint64_t vertex_id) const {
    return m_ext2log.count(vertex_id);
}

double CSR::get_weight(uint64_t source, uint64_t destination) const {
    uint64_t logical_source_id = 0, logical_destination_id = 0;
    try {
        logical_source_id = m_ext2log.at(source);
        logical_destination_id = m_ext2log.at(destination);
    } catch(out_of_range& e){ // either source or destination do not exist
        return numeric_limits<double>::signaling_NaN();
    }

    auto offset = get_out_interval(logical_source_id);
    for(uint64_t i = offset.first, end = offset.second; i < end && m_out_e[i] <= logical_destination_id; i++){
        if(m_out_e[i] == logical_destination_id){
            return m_out_w[i];
        }
    }
//    if(offset.second - offset.first <= 4){
//        for(uint64_t i = offset.first, end = offset.second; i < end && m_out_e[i] <= logical_destination_id; i++){
//            if(m_out_e[i] == logical_destination_id){
//                return m_out_w[i];
//            }
//        }
//    }
//    else {
//        uint32_t loc = lower_bound(m_out_e + offset.first, m_out_e + offset.second, logical_destination_id) - m_out_e;
//        if(m_out_e[loc] == logical_destination_id) {
//            return m_out_w[loc];
//        }
//    }

    return numeric_limits<double>::signaling_NaN();
}

pair<uint64_t, uint64_t> CSR::get_out_interval(uint64_t logical_vertex_id) const {
    return get_interval_impl(m_out_v, logical_vertex_id);
}

pair<uint64_t, uint64_t> CSR::get_in_interval(uint64_t logical_vertex_id) const {
    return get_interval_impl(m_in_v, logical_vertex_id);
}

pair<uint64_t, uint64_t> CSR::get_interval_impl(const uint64_t* __restrict vertex_array, uint64_t logical_vertex_id) const {
    assert(logical_vertex_id < m_num_vertices && "Invalid vertex ID");
    if(logical_vertex_id == 0){
        return make_pair(0ull, vertex_array[0]);
    } else {
        return make_pair(vertex_array[logical_vertex_id -1], vertex_array[logical_vertex_id]);
    }
}

uint64_t CSR::get_out_degree(uint64_t logical_vertex_id) const {
    auto interval = get_out_interval(logical_vertex_id);
    return interval.second - interval.first;
}

uint64_t CSR::get_in_degree(uint64_t logical_vertex_id) const {
    auto interval = get_in_interval(logical_vertex_id);
    return interval.second - interval.first;
}

uint64_t CSR::get_random_vertex_id() const {
    std::mt19937_64 generator { /* seed */ std::random_device{}() };
    std::uniform_int_distribution<uint64_t> distribution{ 0, m_num_vertices -1 };
    uint64_t outcome = distribution(generator);
    return m_log2ext[outcome];
}

void CSR::set_timeout(uint64_t seconds) {
    m_timeout = seconds;
}

uint64_t* CSR::out_v() const { return m_out_v; }
uint64_t* CSR::out_e() const { return m_out_e; }
double* CSR::out_w() const { return m_out_w; }
uint64_t* CSR::in_v() const { return m_in_v; }
uint64_t* CSR::in_e() const { return m_in_e; }
double* CSR::in_w() const { return m_in_w; }



void CSR::load(std::vector<std::pair<uint32_t ,std::pair<uint32_t,double> > >& edges, uint32_t N_vertex, uint64_t M_edge) {

    m_num_edges = M_edge;
    m_num_vertices = N_vertex;
    m_ext2log.reserve(m_num_vertices);
    m_log2ext = alloca_array<uint64_t>(m_num_vertices);

    for(uint64_t i = 0; i < m_num_vertices; i++){
        uint64_t vertex_id = i;
        m_ext2log[vertex_id] = i;
        m_log2ext[i] = vertex_id;
    }

    sort(edges.begin(), edges.end());

    m_out_v = alloca_array<uint64_t>(m_num_vertices); // init to 0
    m_out_e = alloca_array<uint64_t>(m_num_edges);
    m_out_w = alloca_array<double>(m_num_edges);

    for(uint64_t i = 0; i < m_num_edges; i++){
        m_out_v[edges[i].first]++;
        m_out_e[i] = edges[i].second.first;
        m_out_w[i] = edges[i].second.second;
    }

    for(uint64_t i = 1; i < m_num_vertices; i++){
        m_out_v[i] += m_out_v[i -1];
    }
}

std::pair<uint64_t, uint64_t> CSR::traverse(uint64_t logical_vertex_id){
    return get_interval_impl(m_out_v, logical_vertex_id);
}

uint64_t CSR::get_deg(uint64_t logical_vertex_id){
    auto interval = get_out_interval(logical_vertex_id);
    return interval.second - interval.first;
}

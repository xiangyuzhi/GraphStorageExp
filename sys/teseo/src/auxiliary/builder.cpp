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
#include "teseo/auxiliary/builder.hpp"

#include <cassert>

#if defined(HAVE_NUMA)
#include <numa.h>
#endif

#include "teseo/auxiliary/counting_tree.hpp"
#include "teseo/auxiliary/item.hpp"
#include "teseo/auxiliary/partial_result.hpp"
#include "teseo/memstore/key.hpp"
#include "teseo/util/numa.hpp"

//#define DEBUG
#include "teseo/util/debug.hpp"


using namespace std;

namespace teseo::aux {

Builder::Builder() : m_num_partial_results(0), m_num_collected_results(0) {

}

Builder::~Builder(){

    // remove all partial results still in the queue
    auto deleter = [](uint64_t key, PartialResult* partial_result){
        delete partial_result;
        return true;
    };
    m_queue.scan(0, numeric_limits<uint64_t>::max(), deleter);
}

PartialResult* Builder::issue(const memstore::Key& from, const memstore::Key& to) {
    PartialResult* partial_result = new PartialResult(this, m_num_partial_results, from, to);
    m_num_partial_results++;
    return partial_result;
}

void Builder::collect(PartialResult* partial_result){
    assert(partial_result != nullptr && "Null pointer");
    { // restrict the scope
        scoped_lock<mutex> xlock(m_mutex);
        uint64_t id = partial_result->id();
        m_queue.insert(id, partial_result);
    }
    m_condvar.notify_one();
}

PartialResult* Builder::next(){
    auto& queue = m_queue;

    unique_lock<mutex> xlock(m_mutex);
    PartialResult* partial_result = nullptr;
    uint64_t next = m_num_collected_results;
    if(next == m_num_partial_results) return nullptr;
    m_condvar.wait(xlock, [&queue, &partial_result, next](){
        return queue.remove(next, &partial_result);
    });
    m_num_collected_results++;
    return partial_result;
}

ItemUndirected* Builder::create_dv_undirected(uint64_t num_vertices){
    ItemUndirected* __restrict array = (ItemUndirected*) util::NUMA::malloc(num_vertices * sizeof(ItemUndirected)); // it already throws bad::alloc in case of error
    //memset(array, 0, num_vertices * sizeof(ItemUndirected));
    std::fill(array, array + num_vertices, ItemUndirected{}); // make gcc happy

    PartialResult* partial_result = nullptr;
    int64_t pos = -1;
    while( (partial_result = next()) != nullptr ){
        if(!partial_result->empty()){
            if(pos < 0 || partial_result->at(0).m_vertex_id != array[pos].m_vertex_id){ // first index
                pos++;
            }

            for(uint64_t i = 0, end = partial_result->size(); i < end; i++){
                assert(pos >= 0 && "Underflow");
                assert(pos < (int64_t) num_vertices && "Overflow");

                auto item = partial_result->at(i);

                COUT_DEBUG("[" << partial_result << " (id: " << partial_result->id() <<") @ " << i << "] " << item << ", pos: " << pos);
                array[pos].m_vertex_id = item.m_vertex_id;
                array[pos].m_degree += item.m_degree;

                // next iteration
                pos++;
            }

            pos--; // for the next partial result
        }

        // next iteration
        delete partial_result; partial_result = nullptr;
    }

    return array;
}

CountingTree* Builder::create_ct_undirected(){
    CountingTree* ct = new CountingTree();

    PartialResult* partial_result = nullptr;
    uint64_t last_vertex_id = 0;

    while((partial_result = next()) != nullptr ){
        if(!partial_result->empty()){

            // first item
            auto first = partial_result->at(0);
            if(first.m_vertex_id == last_vertex_id){
                ct->get_by_vertex_id(last_vertex_id).first->m_degree += first.m_degree;
            } else {
                ct->insert(first);
            }

            // remaining items
            for(uint64_t i = 1, end = partial_result->size(); i < end; i++){
                ct->insert(partial_result->at(i));
            }

            last_vertex_id = partial_result->at(partial_result->size() -1).m_vertex_id;
        }

        // next iteration
        delete partial_result; partial_result = nullptr;
    }

    return ct;
}

} // namespace



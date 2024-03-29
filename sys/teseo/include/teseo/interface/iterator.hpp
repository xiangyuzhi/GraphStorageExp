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

#include "teseo.hpp"

#include <cassert>
#include <type_traits>

#include "teseo/auxiliary/dynamic_view.hpp"
#include "teseo/auxiliary/static_view.hpp"
#include "teseo/context/global_context.hpp"
#include "teseo/memstore/cursor_state.hpp"
#include "teseo/memstore/error.hpp"
#include "teseo/memstore/memstore.hpp"
#include "teseo/memstore/scan.hpp"
#include "teseo/transaction/transaction_impl.hpp"
#include "teseo/util/interface.hpp"

namespace teseo::interface {

/**
 * Wrapper for an Iterator scan
 */
template<bool logical, bool has_weight_parameter, typename View, typename Callback>
class ScanEdges {
    ScanEdges(const ScanEdges&) = delete;
    ScanEdges& operator=(const ScanEdges&) = delete;

    bool m_vertex_found; // whether we have already traversed the source vertex
    const uint64_t m_vertex_id; // the vertex we are visiting
    transaction::TransactionImpl* m_transaction; // the user transaction
    View const * const m_view; // materialised view to translate the vertex IDs into logical IDs
    memstore::CursorState* m_cursor_state; // cursor instance to resume a sequential scan from the previous point
    const Callback& m_callback; // the user callback, the function ultimately invoked for each visited edge

private:
    // Execute the iterator
    void do_scan(memstore::Memstore* sa);

public:
    // Initialise the instance & start the iterator
    ScanEdges(transaction::TransactionImpl* txn, memstore::Memstore* sa, uint64_t vertex_id, const View* view, memstore::CursorState* cs, const Callback& callback);

    // Trampoline to the user callback
    bool operator()(uint64_t source, uint64_t destination, double weight);
};


template<bool logical, bool has_weight_parameter, typename View, typename Callback>
ScanEdges<logical, has_weight_parameter, View, Callback>::ScanEdges(transaction::TransactionImpl* txn, memstore::Memstore* sa, uint64_t vertex_id, const View* view, memstore::CursorState* cs, const Callback& callback) :
    m_vertex_found(false), m_vertex_id(vertex_id), m_transaction(txn), m_view(view), m_cursor_state(cs), m_callback(callback) {
    do_scan(sa);
}

template<bool logical, bool has_weight_parameter, typename View, typename Callback>
void ScanEdges<logical, has_weight_parameter, View, Callback>::do_scan(memstore::Memstore* sa){
    try {
        if(m_transaction->is_read_only()){
            sa->scan<has_weight_parameter>(m_transaction, m_vertex_id, /* edge destination */ 0, m_cursor_state, *this);
        } else { // read-write transactions
            sa->scan_nolock<has_weight_parameter>(m_transaction, m_vertex_id, /* edge destination */ 0, *this);
        }

        if(!m_vertex_found){
            //MAYBE_BREAK_INTO_DEBUGGER
            throw memstore::Error { memstore::Key { m_vertex_id }, memstore::Error::Type::VertexDoesNotExist };
        }
    } catch( const memstore::Error& error ){
        util::handle_error(error);
    }
};

template<bool logical, bool has_weight_parameter, typename View, typename Callback>
bool ScanEdges<logical, has_weight_parameter, View, Callback>::operator()(uint64_t source, uint64_t destination, double weight){
    if(source != m_vertex_id){
        return false;
    } else if(destination == 0){
        m_vertex_found = true;
        return true;
    } else {
        if(!logical){
            uint64_t external_destination_id = destination -1; // I2E, internally vertices are shifted by +1
            return m_callback(external_destination_id, weight);
        } else {
            uint64_t rank = m_view->logical_id(destination);
            assert(rank != aux::NOT_FOUND && "The destination should always exist");
            return m_callback(rank, weight);
        }
    }
};

template<bool logical, bool has_weight_parameter, typename Callback>
void scan_select_aux_view(transaction::TransactionImpl* txn, memstore::Memstore* sa, uint64_t vertex_id, const aux::View* view, memstore::CursorState* cs, Callback&& callback){
    if(txn->is_read_only()){
        interface::ScanEdges<logical, has_weight_parameter, aux::StaticView, Callback> scan(txn, sa, vertex_id, static_cast<const aux::StaticView*>(view), cs, callback);
    } else {
        interface::ScanEdges<logical, has_weight_parameter, aux::DynamicView, Callback> scan(txn, sa, vertex_id, static_cast<const aux::DynamicView*>(view), cs, callback);
    }
}

// Entry point for the iterator. We're going to normalise the user callback with some pattern matching. Basically the user can provide one of the
// following combinations of signatures for the callable:
// 1- bool (*) (uint64_t destination, double weight); => pass the weight as argument (has_weight_arg = true), use the return value to stop the iterator
// 2- bool (*) (uint64_t destination); => do not pass the weight as argument (has_weight_arg = false), use the return value to stop the iterator
// 3- void (*) (uint64_t destination, double weight); => pass the weight as argument (has_weight_arg = true, never stop the iterator
// 4- void (*) (uint64_t destination); => do not pass the weight as argument (has_weight_arg = false), never stop the iterator

// 1- bool (*) (uint64_t destination, double weight)
template<bool logical, typename Callback>
typename std::enable_if_t< std::is_same_v< std::invoke_result_t<Callback, uint64_t, double>, bool > >
scan_normalise_user_callback(transaction::TransactionImpl* txn, memstore::Memstore* sa, uint64_t vertex_id, const aux::View* view, memstore::CursorState* cs, Callback&& callback) {
    scan_select_aux_view<logical, /* has_weight ? */ true>(txn, sa, vertex_id, view, cs, callback);
}

// 2- bool (*) (uint64_t)
template<bool logical, typename Callback>
typename std::enable_if_t< std::is_same_v< std::invoke_result_t<Callback, uint64_t>, bool > >
scan_normalise_user_callback(transaction::TransactionImpl* txn, memstore::Memstore* sa, uint64_t vertex_id, const aux::View* view, memstore::CursorState* cs, Callback&& callback) {
    auto wrapper = [&callback](uint64_t destination, double weight) {
        assert(weight == 0 && "The user's callback does not feature a parameter for the weight");
        return callback(destination);
    };

    scan_select_aux_view<logical, /* has_weight ? */ false>(txn, sa, vertex_id, view, cs, wrapper);
}

// 3- void (*) (uint64_t, uint64_t, double)
template<bool logical, typename Callback>
typename std::enable_if_t< std::is_same_v< std::invoke_result_t<Callback, uint64_t, double>, void > >
scan_normalise_user_callback(transaction::TransactionImpl* txn, memstore::Memstore* sa, uint64_t vertex_id, const aux::View* view, memstore::CursorState* cs, Callback&& callback) {
    auto wrapper = [&callback](uint64_t destination, double weight){
        callback(destination, weight);
        return true;
    };
    scan_select_aux_view<logical, /* has_weight ? */ true>(txn, sa, vertex_id, view, cs, wrapper);
}

// 4- void (*) (uint64_t)
template<bool logical, typename Callback>
typename std::enable_if_t< std::is_same_v< std::invoke_result_t<Callback, uint64_t>, void > >
scan_normalise_user_callback(transaction::TransactionImpl* txn, memstore::Memstore* sa, uint64_t vertex_id, const aux::View* view, memstore::CursorState* cs, Callback&& callback) {
    auto wrapper = [callback](uint64_t destination, double weight){
        assert(weight == 0 && "The user's callback does not feature a parameter for the weight");
        callback(destination);
        return true;
    };
    scan_select_aux_view<logical, /* has_weight ? */ false>(txn, sa, vertex_id, view, cs, wrapper);
}

} // namespace

namespace teseo  {

// trampoline to the implementation
template<typename Callback>
void Iterator::edges(uint64_t external_vertex_id, bool logical, Callback&& callback) const {
    if(!is_open()) throw LogicalError("LogicalError", "The iterator is closed", __FILE__, __LINE__, __FUNCTION__);
    m_num_alive ++; // to avoid an iterator being closed while in use

    try {
        transaction::TransactionImpl* txn = reinterpret_cast<transaction::TransactionImpl*>(m_pImpl);

        // In case of nesting, we only use the cursor state for the outermost iterator. This is because all nested
        // iterators refer to the same state. As they can overwrite it, we lose the *critical* information of which
        // latches were acquired by the outer iterators.
        memstore::CursorState* cs = (m_num_alive == 1) ? reinterpret_cast<memstore::CursorState*>(m_cursor_state) : nullptr;

        const aux::View* view = nullptr;
        if(txn->has_aux_view() || logical){
            view = txn->aux_view(/* numa aware ? */ true);
        }

        memstore::Memstore* sa = context::global_context()->memstore();
        uint64_t internal_vertex_id = 0;
        if(logical){
            int64_t rank = external_vertex_id;
            internal_vertex_id = view->vertex_id(rank);
            if(internal_vertex_id == aux::NOT_FOUND) throw LogicalError("LogicalError", "Invalid logical vertex", __FILE__, __LINE__, __FUNCTION__);
        } else {
            internal_vertex_id = external_vertex_id +1; // E2I, the vertex ID 0 is reserved, translate all vertex IDs to +1
        }

        if(logical){
            interface::scan_normalise_user_callback<true>(txn, sa, internal_vertex_id, view, cs, callback);
        } else {
            interface::scan_normalise_user_callback<false>(txn, sa, internal_vertex_id, view, cs, callback);
        }

    } catch (...){
        m_num_alive--;
        throw;
    }

    m_num_alive --;
}

} // namespace


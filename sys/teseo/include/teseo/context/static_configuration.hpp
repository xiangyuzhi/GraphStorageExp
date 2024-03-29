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
#include <chrono>
#include <cinttypes>
#include <limits>
#include <string>


namespace teseo::context {



/**
 * Global set of configuration parameters that need to be set at compile time. These
 * parameters are utilised in various parts and components across Teseo.
 */
//    struct StaticConfiguration {
//        /**
//         * [IGNORED] This setting is not used anymore, replaced by `runtime_delay_rebalance'.
//         * The minimum delay between when a request to rebalance is issued by a writer and
//         * when it is finally handled by an asynchronous rebalancer
//         */
//        constexpr static std::chrono::milliseconds async_delay { 200 }; // ms
//
//        /**
//         * [IGNORED] This setting is not used anymore.
//         * Number of background threads performing rebalances on the background
//         */
//        constexpr static uint64_t async_num_threads = @conf_async_num_threads@;
//
//        /**
//         * Whether to cache the last computed auxiliary view, so that it can be reused among multiple
//         * transactions.
//         *
//         * This setting can also be enabled or disable at runtime using:
//         * - context::global_context()->enable_aux_cache();
//         * - context::global_context()->disable_aux_cache();
//         */
//        constexpr static bool aux_cache_enabled = true;
//
//        /**
//         * The maximum number of separator keys that can be stored in the internal node of the counting trees,
//         * used by the dynamic views.
//         */
//        constexpr static uint64_t aux_counting_tree_capacity_inodes = @conf_aux_counting_tree_capacity_inodes@;
//
//        /**
//         * The maximum number of elements that can be stored in the leaves of the counting trees, used by the
//         * dynamic views.
//         */
//        constexpr static uint64_t aux_counting_tree_capacity_leaves = @conf_aux_counting_tree_capacity_leaves@;
//
//        /**
//         * Whether it is allowed to use the auxiliary view to answer queries on the degree of a vertex.
//         *
//         * This setting can also be enabled or disable at runtime using:
//         * - context::global_context()->enable_aux_degree();
//         * - context::global_context()->disable_aux_degree();
//         */
//        constexpr static bool aux_degree_enabled = true;
//
//        /**
//         * Automatically create an auxiliary view after the user queried a vertex degree for the Nth time.
//         */
//        constexpr static uint64_t aux_degree_threshold = 10;
//
//        /**
//         * Map the logical vertices into an array rather than a hash table when convenient to do so.
//         * Only applicable to read-only transactions and static auxiliary views.
//         */
//        constexpr static bool aux_direct_table_enabled = true;
//
//        /**
//         * Set the multiplier for the hash dictionary used to translate the vertex IDs into
//         * logical IDs in the static views. The multiplier `m' defines the ultimate capacity
//         * of the hash table and is the reciprocal of the load factor (1/m).
//         */
//        constexpr static uint64_t aux_hash_multiplier = 4;
//        static_assert(aux_hash_multiplier > 1);
//
//        /**
//         * The initial capacity of the containers, in terms of number of vertices, used in the
//         * intermediates to create the auxiliary view. The capacity can still be further increased
//         * eventually at runtime upon necessity.
//         */
//        constexpr static uint64_t aux_partial_init_capacity = 4096;
//
//        /**
//         * The capacity of the thread contexts list upon creation (it can be further expanded at runtime).
//         */
//        constexpr static uint64_t context_tclist_initial_capacity = @conf_tclist_initial_capacity@;
//
//        /**
//         * The height of the calibrator tree [0 => log2(num_segments)].
//         */
//        constexpr static uint64_t crawler_calibrator_tree_height = @conf_crawler_calibrator_tree_height@;
//
//        /**
//         * The initial capacity of a queue in the GC, in terms of number of entries.
//         */
//        constexpr static uint64_t gc_queue_initial_capacity = 1024;
//
//        /**
//         * Whether to explicitly store the pivot in a sparse file at the start of the segment. This is a
//         * mere optimisation aimed at reducing the amount of random accesses in point lookups. When enabled,
//         * the capacity of a sparse file is reduced by 16 bytes.
//         */
//        constexpr static bool memstore_duplicate_pivot = true;
//
//        /**
//         * The maximum number of segments in each leaf of the memstore. Leaves in a fat tree are variable sized,
//         * between [M/2, M] segments.
//         */
//        constexpr static uint64_t memstore_max_num_segments_per_leaf = @conf_memstore_max_num_segments_per_leaf@;
//
//        /**
//         * The size of the first block in the payload file, as a multiple of sizeof(uint64_t)
//         */
//        constexpr static uint64_t memstore_payload_file_first_block_size = @conf_memstore_payload_file_first_block_size@;
//
//        /**
//         * The size of the all subsequent blocks, after the first, in the payload file, as a multiple of size(uint64_t)
//         */
//        constexpr static uint64_t memstore_payload_file_next_block_size = @conf_memstore_payload_file_next_block_size@;
//
//        /**
//         * Enable explicit cache prefetching for point lookups.
//         */
//        constexpr static bool memstore_prefetch = true;
//
//        /**
//         * The size of each segment, as multiple of sizeof(uint64_t).
//         */
//        constexpr static uint64_t memstore_segment_size = @conf_memstore_segment_size@;
//
//        /**
//         * How often to execute the merger service.
//         */
//        constexpr static std::chrono::milliseconds merger_frequency { 1000 * 20 }; // 20 secs
//
//        /**
//         * Whether to be NUMA aware when allocating memory. Currently only used by the static auxiliary
//         * views as an optimisation.
//         */
//        constexpr static bool numa_enabled = @conf_numa_enabled@;
//
//        /**
//         * Number of NUMA nodes, i.e. sockets, in the underlying hardware.
//         */
//        constexpr static uint64_t numa_num_nodes = @conf_numa_num_nodes@;
//        static_assert(numa_num_nodes >= 1);
//
//        /**
//         * How often to check whether some physical memory can be released from the buffer pool.
//         */
//        constexpr static std::chrono::milliseconds runtime_bp_frequency { 1000 }; // 1 sec
//
//        /**
//         * The minimum delay between when a request to rebalance is issued by a writer and
//         * when it is handled by an asynchronous rebalancer.
//         */
//        constexpr static std::chrono::milliseconds runtime_delay_rebalance { 200 }; // ms
//
//        /**
//         * How often to execute the garbage collector
//         */
//        constexpr static std::chrono::milliseconds runtime_gc_frequency { 20 }; // ms
//
//        /**
//         * Number of background threads performing the maintenance tasks. The value 0 will automatically
//         * set a default number of threads at runtime, depending on the underlying machine.
//         */
//        constexpr static uint64_t runtime_num_threads = @conf_runtime_num_threads@;
//
//        /**
//         * How often to refresh a cached transaction list.
//         */
//        constexpr static std::chrono::milliseconds runtime_txnlist_refresh { @conf_runtime_txnlist_refresh@ }; // ms
//
//        /**
//         * Frequency to rebuild the free list in the cached transaction pools.
//         */
//        constexpr static std::chrono::milliseconds runtime_txnpool_frequency { 300 }; // ms
//
//        /**
//         * Test mode is a configuration profile enabled by autoconf, where segments, leaves and other internal
//         * parameters are smaller, if not tiny. Test mode is required to run the testsuite, analyse and make
//         * propert assertions on the internal state of Teseo, and debug issues without compromising our own
//         * physical and mental sanity when dealing with the actual sizes employed in the final builds.
//         */
//        constexpr static bool test_mode = @test_mode@;
//
//        /**
//         * The fill factor, in [0, 1], on when a memory pool can be reused by another thread.
//         */
//        constexpr static double transaction_memory_pool_ffreuse = 0.1;
//
//        /**
//         * Max number of memory pools that can be cached, ready to be reused.
//         */
//        constexpr static uint64_t transaction_memory_pool_list_cache_size = 8;
//
//        /**
//         * The capacity of each memory pool, in terms of number of transactions.
//         */
//        constexpr static uint32_t transaction_memory_pool_size = @conf_transaction_memory_pool_size@;
//
//        /**
//         * The default size, in bytes, of an undo buffer created by a transaction after the embedded
//         * buffer is exhausted.
//         */
//        constexpr static uint32_t transaction_undo_buffer_size = 4096;
//
//        /**
//         * The size, in bytes, of the first undo buffer stored inside a transaction. When a transaction
//         * is created, we always embed a small undo buffer with it of the size below.
//         */
//        constexpr static uint32_t transaction_undo_embedded_size = 128; // space for 2 updates
//
//        /**
//         * The maximum fill factor of the vertex table before triggering a resize
//         */
//        constexpr static double vertex_table_max_fill_factor = 0.6; // 60%
//
//        /**
//         * The minimum capacity of the vertex table
//         */
//        constexpr static uint64_t vertex_table_min_capacity = @conf_vertex_table_min_capacity@;
//    };

    /*
if test x"${enable_test}" == x"yes"; then
    test_mode="true";
    conf_async_num_threads="2";
    conf_aux_counting_tree_capacity_inodes="5";
    conf_aux_counting_tree_capacity_leaves="4";
    conf_crawler_calibrator_tree_height="0";
    conf_memstore_max_num_segments_per_leaf="4";
    conf_memstore_payload_file_first_block_size="16";
    conf_memstore_payload_file_next_block_size="16";
    conf_memstore_segment_size="34"; # 1 for the header, 2 for the cached pivot, 31 for the content
    conf_runtime_num_threads="2";
    conf_runtime_txnlist_refresh="1"; # millisecs
    conf_tclist_initial_capacity="4";
    conf_transaction_memory_pool_size="8";
    conf_vertex_table_min_capacity="2";
     */
    // test
//    struct StaticConfiguration {
//        constexpr static std::chrono::milliseconds async_delay { 200 }; // ms
//        constexpr static uint64_t async_num_threads = 2;
//        constexpr static bool aux_cache_enabled = true;
//        constexpr static uint64_t aux_counting_tree_capacity_inodes = 5;
//        constexpr static uint64_t aux_counting_tree_capacity_leaves = 4;
//        constexpr static bool aux_degree_enabled = true;
//        constexpr static uint64_t aux_degree_threshold = 10;
//        constexpr static bool aux_direct_table_enabled = true;
//        constexpr static uint64_t aux_hash_multiplier = 4;
//        static_assert(aux_hash_multiplier > 1);
//        constexpr static uint64_t aux_partial_init_capacity = 4096;
//        constexpr static uint64_t context_tclist_initial_capacity = 4;
//        constexpr static uint64_t crawler_calibrator_tree_height = 0;
//        constexpr static uint64_t gc_queue_initial_capacity = 1024;
//        constexpr static bool memstore_duplicate_pivot = true;
//        constexpr static uint64_t memstore_max_num_segments_per_leaf = 4;
//        constexpr static uint64_t memstore_payload_file_first_block_size = 16;
//        constexpr static uint64_t memstore_payload_file_next_block_size = 16;
//        constexpr static bool memstore_prefetch = true;
//        constexpr static uint64_t memstore_segment_size = 34;
//        constexpr static std::chrono::milliseconds merger_frequency { 1000 * 20 }; // 20 secs
//        constexpr static bool numa_enabled = false;
//        constexpr static uint64_t numa_num_nodes = 1;
//        static_assert(numa_num_nodes >= 1);
//        constexpr static std::chrono::milliseconds runtime_bp_frequency { 1000 }; // 1 sec
//        constexpr static std::chrono::milliseconds runtime_delay_rebalance { 200 }; // ms
//        constexpr static std::chrono::milliseconds runtime_gc_frequency { 20 }; // ms
//        constexpr static uint64_t runtime_num_threads = 2;
//        constexpr static std::chrono::milliseconds runtime_txnlist_refresh { 1 }; // ms
//        constexpr static std::chrono::milliseconds runtime_txnpool_frequency { 300 }; // ms
//        constexpr static bool test_mode = true;
//        constexpr static double transaction_memory_pool_ffreuse = 0.1;
//        constexpr static uint64_t transaction_memory_pool_list_cache_size = 8;
//        constexpr static uint32_t transaction_memory_pool_size = 8;
//        constexpr static uint32_t transaction_undo_buffer_size = 4096;
//        constexpr static uint32_t transaction_undo_embedded_size = 128; // space for 2 updates
//        constexpr static double vertex_table_max_fill_factor = 0.6; // 60%
//        constexpr static uint64_t vertex_table_min_capacity = 2;
//    };
/*
else
    test_mode="false";
    conf_async_num_threads="8";
    conf_aux_counting_tree_capacity_inodes="63";
    conf_aux_counting_tree_capacity_leaves="64";
    conf_crawler_calibrator_tree_height="0";
    conf_memstore_max_num_segments_per_leaf="128";
    conf_memstore_payload_file_first_block_size="510"; # 2 words for the header
    conf_memstore_payload_file_next_block_size="254"; # 2 words for the header
    conf_memstore_segment_size="512";
    conf_runtime_num_threads="0";
    conf_runtime_txnlist_refresh="60"; # millisecs
    conf_tclist_initial_capacity="128";
    conf_transaction_memory_pool_size="1<<16";
    conf_vertex_table_min_capacity="1<<19";
 */
    // test
    struct StaticConfiguration {
        constexpr static std::chrono::milliseconds async_delay { 200 }; // ms
        constexpr static uint64_t async_num_threads = 8;
        constexpr static bool aux_cache_enabled = true;
        constexpr static uint64_t aux_counting_tree_capacity_inodes = 63;
        constexpr static uint64_t aux_counting_tree_capacity_leaves = 64;
        constexpr static bool aux_degree_enabled = true;
        constexpr static uint64_t aux_degree_threshold = 10;
        constexpr static bool aux_direct_table_enabled = true;
        constexpr static uint64_t aux_hash_multiplier = 4;
        static_assert(aux_hash_multiplier > 1);
        constexpr static uint64_t aux_partial_init_capacity = 4096;
        constexpr static uint64_t context_tclist_initial_capacity = 128;
        constexpr static uint64_t crawler_calibrator_tree_height = 0;
        constexpr static uint64_t gc_queue_initial_capacity = 1024;
        constexpr static bool memstore_duplicate_pivot = true;
        constexpr static uint64_t memstore_max_num_segments_per_leaf = 128;
        constexpr static uint64_t memstore_payload_file_first_block_size = 510;
        constexpr static uint64_t memstore_payload_file_next_block_size = 254;
        constexpr static bool memstore_prefetch = true;
        constexpr static uint64_t memstore_segment_size = 512;
        constexpr static std::chrono::milliseconds merger_frequency { 1000 * 20 }; // 20 secs
        constexpr static bool numa_enabled = false;
        constexpr static uint64_t numa_num_nodes = 1;
        static_assert(numa_num_nodes >= 1);
        constexpr static std::chrono::milliseconds runtime_bp_frequency { 1000 }; // 1 sec
        constexpr static std::chrono::milliseconds runtime_delay_rebalance { 200 }; // ms
        constexpr static std::chrono::milliseconds runtime_gc_frequency { 20 }; // ms
        constexpr static uint64_t runtime_num_threads = 0;
        constexpr static std::chrono::milliseconds runtime_txnlist_refresh { 60 }; // ms
        constexpr static std::chrono::milliseconds runtime_txnpool_frequency { 300 }; // ms
        constexpr static bool test_mode = true;
        constexpr static double transaction_memory_pool_ffreuse = 0.1;
        constexpr static uint64_t transaction_memory_pool_list_cache_size = 8;
        constexpr static uint32_t transaction_memory_pool_size = 1<<16;
        constexpr static uint32_t transaction_undo_buffer_size = 4096;
        constexpr static uint32_t transaction_undo_embedded_size = 128; // space for 2 updates
        constexpr static double vertex_table_max_fill_factor = 0.6; // 60%
        constexpr static uint64_t vertex_table_min_capacity = 1<<19;
    };
} // namespace

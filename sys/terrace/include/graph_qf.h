/*
 * ============================================================================
 *
 *       Filename:  graph.h
 *
 *         Author:  Prashant Pandey (), ppandey2@cs.cmu.edu
 *   Organization:  Carnegie Mellon University
 *
 * ============================================================================
 */

#ifndef __GRAPH_QF_H__
#define __GRAPH_QF_H__

#include <stdlib.h>

#include <unordered_set>
#include <vector>
#include <iostream>

#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <sdsl/util.hpp>

#include "gqf_cpp.h"

namespace graphstore {

#define DEFAULT_SIZE (1ULL << 24)
#define KEYBITS 40

	// An efficient way to store graph topology for sparse (and skewed) DAGs. 
	// Vertices are defined as 32-bit integers.
	// There can be only one edge between two vertices, i.e., simple graphs.
	// Each vertex and up to 8 neighbors are stored in place in the QF as
	// <key,value> pairs.
	// If there are more than 8 neighbors then they are stored in a
	// ordered container and a pointer is stored next to the source vertex in
	// the QF.
	class Graph {
		public:
			typedef uint32_t vertex;
			typedef std::pair<vertex, vertex> edge;
			typedef std::set<vertex> vertex_set;
			typedef std::set<edge> edge_set;
			typedef vertex_set::iterator vertex_set_iterator;

			Graph();	// create a graph with a default size
			Graph(uint32_t size);	// create a graph with the given size (#num edges)
			Graph(std::string prefix);	// read graph from the ifstream

			~Graph();

			// add edge between vertices s and d
			// if edge already exists will not do anything.
			int add_edge(const vertex s, const vertex d);
			//int add_edge_new(const vertex s, const vertex d);
			// remove edge between vertices s and d
			// if edge doesn't exists will not do anything.
			int remove_edge(const vertex s, const vertex d);
			//int remove_edge_new(const vertex s, const vertex d);

			// get out neighbors of vertex s
			vertex_set out_neighbors(const vertex v) const;
			//vertex_set out_neighbors_new(const vertex v) const;
			// get out degree of vertex v
			uint32_t out_degree(const vertex v) const;
			//uint32_t out_degree_new(const vertex v) const;

			// TODO these two functions are not implemented currently.
			vertex_set in_neighbors(const vertex v) const;
			uint32_t in_degree(const vertex v) const;

			// returns true if edge e exists in the graph. false otherwise.
			bool is_edge(const edge e) const;
			//bool is_edge_new(const edge e) const;

			uint32_t get_num_vertices(void) const;
			uint32_t get_num_edges(void) const;

			// serialize graph to file
			void serialize(std::string prefix);

			class GraphIterator {
				public:
					GraphIterator(const Graph* graph, vertex v, uint64_t radius);
					vertex operator*(void) const;
					void operator++(void);
					bool done(void) const;

				private:
					vertex cur;
					uint64_t r;
					bool is_done;
					const Graph* g;
					std::queue<std::pair<Graph::vertex, uint64_t>> q;
					std::unordered_set<Graph::vertex> visited;
			};

			GraphIterator find(vertex v = 0, uint64_t radius = UINT64_MAX); 

			// Iterate over all nodes in the graph
			class VertexIterator {
				public:
					VertexIterator(CQF<KeyObject>::Iterator it);
					vertex operator*(void) const;
					void operator++(void);
					bool done(void) const;

				private:
					CQF<KeyObject>::Iterator adj_list_itr;
			};

			VertexIterator begin_vertices(void) const;
			VertexIterator end_vertices(void) const;

			// Iterate over all edges in the graph
			class EdgeIterator {
				public:
					EdgeIterator(CQF<KeyObject>::Iterator it, const Graph &g);
					edge operator*(void) const;
					void operator++(void);
					bool done(void) const;

				private:
					void init_vertex_set_and_itr(void);
					void init_vertex_set_and_itr_new(void);

					const Graph &g;
					CQF<KeyObject>::Iterator adj_list_itr;
					vertex_set cur_vertex_set;
					vertex_set_iterator vertex_set_itr;
					uint32_t is_inplace{0};
			};

			EdgeIterator begin_edges(void) const;
			EdgeIterator end_edges(void) const;

		private:
			std::vector<vertex_set> aux_vertex_list;
			uint32_t num_vertices{0};
			uint32_t num_edges{0};
			CQF<KeyObject> adj_list;
	};

	Graph::Graph() {
		adj_list = CQF<KeyObject>(DEFAULT_SIZE, KEYBITS, 1, QF_HASH_INVERTIBLE);
		adj_list.set_auto_resize();
	}

	Graph::Graph(uint32_t size) {
		adj_list = CQF<KeyObject>(size, KEYBITS, 1, QF_HASH_INVERTIBLE);
		adj_list.set_auto_resize();
	}

	Graph::Graph(std::string prefix) {
		std::string adj_list_name("/adj_list.cqf");
		std::string vertex_list_name("/aux_vertex_list.sdsl");
		std::string list_lengths_name("/aux_vertex_list_lengths.sdsl");
		// load cqf
		adj_list = CQF<KeyObject>(prefix + adj_list_name, FREAD);

		// create sdsl vectors
		sdsl::int_vector<32> vertex_list;
		sdsl::int_vector<32> list_lengths;
		sdsl::load_from_file(vertex_list, prefix + vertex_list_name);
		sdsl::load_from_file(list_lengths, prefix + list_lengths_name);

		uint64_t v_idx = 0;
		for (auto l_itr = list_lengths.begin(); l_itr != list_lengths.end();
				 ++l_itr) {
			uint64_t size = *l_itr;
			vertex_set v_set;	
			for (uint64_t pos = v_idx; pos < v_idx + size; ++pos)
				v_set.insert(vertex_list[pos]);
			aux_vertex_list.emplace_back(v_set);
			v_idx += size;
		}
	}

	Graph::~Graph() {
		adj_list.destroy();
		aux_vertex_list.clear();
	}

	void Graph::serialize(std::string prefix) {
		std::string adj_list_name("/adj_list.cqf");
		std::string vertex_list_name("/aux_vertex_list.sdsl");
		std::string list_lengths_name("/aux_vertex_list_lengths.sdsl");
		// serialize the cqf
		adj_list.serialize(prefix + adj_list_name);
		// determine the size of aux_vertex_list
		uint64_t total_len = 0;
		for (const auto list : aux_vertex_list)
			total_len += list.size();

		// create sdsl vectors
		sdsl::int_vector<32> vertex_list(total_len);
		sdsl::int_vector<32> list_lengths(aux_vertex_list.size());

		// populate sdsl vectors
		uint64_t v_idx = 0, l_idx = 0;
		for (const auto list : aux_vertex_list) {
			for (const auto v : list)
				vertex_list[v_idx++] = v;
			list_lengths[l_idx++] = list.size();
		}
		// compress and serialize
		vertex_list.resize(vertex_list.size());
		list_lengths.resize(list_lengths.size());
		//sdsl::util::bit_compress(vertex_list);
		//sdsl::util::bit_compress(list_lengths);
		sdsl::store_to_file(vertex_list, prefix + vertex_list_name);
		sdsl::store_to_file(list_lengths, prefix + list_lengths_name);
	}

	int Graph::add_edge(const vertex s, const vertex d) {
		uint64_t is_inplace {0};
		vertex val = adj_list.query_key(KeyObject(s, 0, 0), &is_inplace,
																		QF_NO_LOCK);
		if (d == 0)
			return 0;
		if (is_inplace == 1 && val == d)	// edge already exist
			return 0;	
		else if (val == 0) 	{	// new vertex
			num_edges++;
			num_vertices++;
			return adj_list.insert(KeyObject(s, 1, d), QF_NO_LOCK);
		}
		else {		// existing node
			if (is_inplace == 1) { // there's only one outgoing edge
				// create a new vector and add outgoing vertices 
				vertex_set neighbors;
				neighbors.insert(val);
				neighbors.insert(d);
				aux_vertex_list.emplace_back(neighbors);
				uint32_t pointer = aux_vertex_list.size();	// it is an offset in the vector. 
				num_edges++;
				// the pointer is always incremented by 1
				return adj_list.replace_key(KeyObject(s, 1, val), KeyObject(s, 0, pointer),
																		QF_NO_LOCK);
			} else {
				if (aux_vertex_list[val - 1].insert(d).second)
					num_edges++;
			}
		}
		return 0;
	}

	//int Graph::add_edge_new(const vertex s, const vertex d) {
		//KeyObject key(s, 0, 0);
		//auto key_itr = adj_list.begin(key);
		//uint32_t local_neighbors{0};
		//uint32_t offset{0};

		//KeyObject cur;
		//while (!key_itr.done()) {
			//local_neighbors++;
			//cur = *key_itr;
			//if (cur.value == d) // edge already exist
				//return 0;
			//if (cur.count == 2) {
				//offset = cur.value - 1;
			//}
			//++key_itr;
		//}

		//if (local_neighbors == 0) {
			//num_vertices++;
		//}
		//if (local_neighbors < NUM_LOCAL_NEIGHBORS) { // add edge as local neighbor
			//num_edges++;
			//return adj_list.insert(KeyObject(s, d, 1), QF_NO_LOCK);
		//} else if (local_neighbors == NUM_LOCAL_NEIGHBORS) { // add edge container
			//vertex_set neighbors;
			//neighbors.insert(d);
			//aux_vertex_list.emplace_back(neighbors);
			//uint32_t pointer = aux_vertex_list.size();	// it is an offset in the vector. 
			//num_edges++;
			//return adj_list.insert(KeyObject(s, pointer, 2), QF_NO_LOCK);
		//} else { // add to the container
			//if (aux_vertex_list[offset].insert(d).second)
				//num_edges++;
		//}

		//return 0;
	//}

	int Graph::remove_edge(const vertex s, const vertex d) {
		uint64_t is_inplace {0};
		vertex val = adj_list.query_key(KeyObject(s, 0, 0), &is_inplace,
																		QF_NO_LOCK);
		if (val == 0)
			return 0;
		else {
			if (is_inplace == 1) {
				return adj_list.delete_key(KeyObject(s, 1, d), QF_NO_LOCK);
			} else {
				for (auto const vertex : aux_vertex_list[val - 1]) {
					if (vertex == d) {
						aux_vertex_list[val - 1].erase(aux_vertex_list[val - 1].begin());
						break;
					}
				}
				//if (aux_vertex_list[val - 1].size() == 0)
					//return adj_list.delete_key(KeyObject(s, 0, 0), QF_NO_LOCK);
			}
		}
		return 0;
	}
	
	//int Graph::remove_edge_new(const vertex s, const vertex d) {
		//KeyObject key(s, 0, 0);
		//auto key_itr = adj_list.begin(key);
		//uint32_t local_neighbors{0};
		//uint32_t offset{0};

		//KeyObject cur;
		//while (!key_itr.done()) {
			//local_neighbors++;
			//cur = *key_itr;
			//if (cur.value == d) {
				//return adj_list.delete_key(KeyObject(s, d, 1), QF_NO_LOCK);
			//}
			//if (cur.count == 2) {
				//offset = cur.value - 1;
			//}
			//++key_itr;
		//}

		//if (local_neighbors <= NUM_LOCAL_NEIGHBORS) { // edge not found
			//return 0;
		//} else {
			//for (auto const vertex : aux_vertex_list[offset]) {
				//if (vertex == d) {
					//aux_vertex_list[offset].erase(aux_vertex_list[offset].begin());
					//break;
				//}
			//}
		//}

		//return 0;
	//}

	Graph::vertex_set Graph::out_neighbors(const vertex v) const {
		Graph::vertex_set neighbor_set;
		uint64_t is_inplace {0};
		vertex val = adj_list.query_key(KeyObject(v, 0, 0), &is_inplace,
																		QF_NO_LOCK);
		if (val == 0)
			return neighbor_set;
		else {
			if (is_inplace == 1) {
				neighbor_set.insert(val);
			} else {
				neighbor_set = aux_vertex_list[val - 1];
			}
			return neighbor_set;
		}
	}
	
	//Graph::vertex_set Graph::out_neighbors_new(const vertex v) const {
		//Graph::vertex_set neighbor_set;
		//KeyObject key(v, 0, 0);
		//auto key_itr = adj_list.begin(key);
		//uint32_t local_neighbors{0};
		//uint32_t offset{0};

		//KeyObject cur;
		//while (!key_itr.done()) {
			//local_neighbors++;
			//cur = *key_itr;
			//neighbor_set.insert(cur.value);
			//if (cur.count == 2) {
				//offset = cur.value - 1;
			//}
			//++key_itr;
		//}

		//if (local_neighbors <= NUM_LOCAL_NEIGHBORS) { // no more edges 
			//return neighbor_set;
		//} else { // additional edges
			//neighbor_set.insert(aux_vertex_list[offset].begin(),
													//aux_vertex_list[offset].end());
			//return neighbor_set;
		//}
	//}

	uint32_t Graph::out_degree(const vertex v) const {
		uint64_t is_inplace {0};
		vertex val = adj_list.query_key(KeyObject(v, 0, 0), &is_inplace,
																		QF_NO_LOCK);
		if (val == 0)
			return 0;
		else {
			if (is_inplace == 1) {
				return 1;	
			} else {
				return aux_vertex_list[val - 1].size();
			}
		}
		return 0;
	}
	
	//uint32_t Graph::out_degree_new(const vertex v) const {
		//KeyObject key(v, 0, 0);
		//auto key_itr = adj_list.begin(key);
		//uint32_t local_neighbors{0};
		//uint32_t offset{0};

		//KeyObject cur;
		//while (!key_itr.done()) {
			//local_neighbors++;
			//if (cur.count == 2) {
				//offset = cur.value - 1;
			//}
			//++key_itr;
		//}

		//if (local_neighbors <= NUM_LOCAL_NEIGHBORS) { // no more edges 
			//return local_neighbors;
		//} else { // additional edges
			//return local_neighbors + aux_vertex_list[offset].size();
		//}
	//}

	bool Graph::is_edge(const edge e) const {
		Graph::vertex_set neighbor_set;
		uint64_t is_inplace {0};
		vertex val = adj_list.query_key(KeyObject(e.first, 0, 0), &is_inplace,
																		QF_NO_LOCK);
		if (val == 0)
			return false;
		else {
			if (is_inplace == 1) {
				if (val == e.second)
					return true;
			} else {
				auto itr = aux_vertex_list[val - 1].find(e.second);
				if (itr != aux_vertex_list[val - 1].end())
					return true;
			}
		}
		return false;
	}
	
	//bool Graph::is_edge_new(const edge e) const {
		//Graph::vertex_set neighbor_set;
		//KeyObject key(e.first, 0, 0);
		//auto key_itr = adj_list.begin(key);
		//uint32_t local_neighbors{0};
		//uint32_t offset{0};

		//KeyObject cur;
		//while (!key_itr.done()) {
			//local_neighbors++;
			//cur = *key_itr;
			//if (cur.value == e.second)
				//return true;
			//if (cur.count == 2) {
				//offset = cur.value - 1;
			//}
			//++key_itr;
		//}

		//if (local_neighbors <= NUM_LOCAL_NEIGHBORS) { // no more edges 
			//return false;
		//} else { // additional edges
			//auto itr = aux_vertex_list[offset].find(e.second);
			//if (itr != aux_vertex_list[offset].end())
				//return true;
		//}

		//return false;
	//}

	uint32_t Graph::get_num_vertices(void) const {
		return num_vertices;
	}

	uint32_t Graph::get_num_edges(void) const {
		return num_edges;
	}

	Graph::VertexIterator Graph::begin_vertices(void) const {
		return VertexIterator(adj_list.begin());
	}

	Graph::VertexIterator Graph::end_vertices(void) const {
		return VertexIterator(adj_list.end());
	}

	Graph::VertexIterator::VertexIterator(CQF<KeyObject>::Iterator it) :
		adj_list_itr(it) {};

	Graph::vertex Graph::VertexIterator::operator*(void) const {
		return (*adj_list_itr).key;
	}

	void Graph::VertexIterator::operator++(void) {
		++adj_list_itr;
	}

	bool Graph::VertexIterator::done(void) const {
		return adj_list_itr.done();
	}

	Graph::EdgeIterator Graph::begin_edges(void) const {
		return EdgeIterator(adj_list.begin(), *this);
	}

	Graph::EdgeIterator Graph::end_edges(void) const {
		return EdgeIterator(adj_list.end(), *this);
	}

	void Graph::EdgeIterator::init_vertex_set_and_itr(void) {
		is_inplace = (*adj_list_itr).value;
		if (is_inplace == 0) {
			cur_vertex_set = g.out_neighbors((*adj_list_itr).key);
		} else {
			cur_vertex_set.clear();
			cur_vertex_set.insert((*adj_list_itr).count);
		}
		vertex_set_itr = cur_vertex_set.begin();
	}
	
	Graph::EdgeIterator::EdgeIterator(CQF<KeyObject>::Iterator it, const Graph
																		&g) : g(g), adj_list_itr(it) {
		init_vertex_set_and_itr();
	};

	Graph::edge Graph::EdgeIterator::operator*(void) const {
		return edge((*adj_list_itr).key, *vertex_set_itr);
	}

	void Graph::EdgeIterator::operator++(void) {
		++vertex_set_itr;
		if (vertex_set_itr == cur_vertex_set.end()) {
			KeyObject cur_key = *adj_list_itr;
			do {
				++adj_list_itr;
			} while (cur_key.key == (*adj_list_itr).key);
			init_vertex_set_and_itr();
		}
	}

	bool Graph::EdgeIterator::done(void) const {
		return adj_list_itr.done();
	}

	Graph::GraphIterator::GraphIterator(const Graph* graph, vertex v, uint64_t
																			radius) {
		g = graph; 
		cur = v;
		visited.insert(v);
		r = radius;
		is_done = false;
		// add neighbors of v to the queue.
		if (radius > 0) {
			for (const auto v : g->out_neighbors(v)) {
				q.push(std::make_pair(v, 1));
			}
		}
	}

	Graph::vertex Graph::GraphIterator::operator*(void) const {
		return cur;
	}

	void Graph::GraphIterator::operator++(void) {
		Graph::vertex cur_vertex = 0;
		uint64_t hop = 0;
		while (!q.empty()) {
			cur_vertex = q.front().first;
			hop = q.front().second;
			if (visited.find(cur_vertex) == visited.end()) {
				visited.insert(cur_vertex);
				break;
			}
			else
				q.pop();
		}
		if (q.empty()) {
			is_done = true;
			return;
		}
		cur = cur_vertex;
		q.pop();
		if (hop < r) {
			for (const auto v : g->out_neighbors(cur))
				q.push(std::make_pair(v, hop + 1));
		}
	}

	bool Graph::GraphIterator::done(void) const {
		return is_done;
	}
	
	Graph::GraphIterator Graph::find(vertex v, uint64_t radius) {
		return GraphIterator(this, v, radius);
	}
}

#endif	// __GRAPH_QF_H__

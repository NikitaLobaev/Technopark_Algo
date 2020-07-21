#include "SetGraph.hpp"

SetGraph::SetGraph(size_t vertices_count): sets(vertices_count) {
}
SetGraph::SetGraph(const IGraph &graph): sets(graph.vertices_count()) {
	for (unsigned int iter_vertex = 0; iter_vertex < sets.size(); iter_vertex++) {
		for (const unsigned int &adjacent : graph.get_next_vertices(iter_vertex)) {
			sets[iter_vertex].insert(adjacent);
		}
	}
}

void SetGraph::add_edge(unsigned int from, unsigned int to) {
	sets[from].insert(to);
}

size_t SetGraph::vertices_count() const {
	return sets.size();
}

std::vector<unsigned int> SetGraph::get_next_vertices(unsigned int vertex) const {
	return std::vector<unsigned int>(sets[vertex].begin(), sets[vertex].end());
}

std::vector<unsigned int> SetGraph::get_prev_vertices(unsigned int vertex) const {
	std::vector<unsigned int> result;
	for (unsigned int iter_vertex = 0; iter_vertex < vertices_count(); iter_vertex++) {
		if (sets[iter_vertex].find(vertex) != sets[iter_vertex].end()) {
			result.push_back(iter_vertex);
		}
	}
	return result;
}

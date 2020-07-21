#include "ArcGraph.hpp"

ArcGraph::ArcGraph(size_t vertices_count): vertices_count_(vertices_count) {
}

ArcGraph::ArcGraph(const IGraph &graph): vertices_count_(graph.vertices_count()) {
	for (unsigned int iter_vertex = 0; iter_vertex < vertices_count_; iter_vertex++) {
		for (const unsigned int &adjacent : graph.get_next_vertices(iter_vertex)) {
			edges.emplace_back(iter_vertex, adjacent);
		}
	}
}

void ArcGraph::add_edge(unsigned int from, unsigned int to) {
	edges.emplace_back(from, to);
}

size_t ArcGraph::vertices_count() const {
	return vertices_count_;
}

std::vector<unsigned int> ArcGraph::get_next_vertices(unsigned int vertex) const {
	std::vector<unsigned int> result;
	for (const std::pair<unsigned int, unsigned int> &edge : edges) {
		if (edge.first == vertex) {
			result.push_back(edge.second);
		}
	}
	return result;
}

std::vector<unsigned int> ArcGraph::get_prev_vertices(unsigned int vertex) const {
	std::vector<unsigned int> result;
	for (const std::pair<unsigned int, unsigned int> &edge : edges) {
		if (edge.second == vertex) {
			result.push_back(edge.first);
		}
	}
	return result;
}

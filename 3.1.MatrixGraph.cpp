#include "MatrixGraph.hpp"

MatrixGraph::MatrixGraph(size_t vertices_count): vertices_count_(vertices_count),
adjacency_matrix(vertices_count * vertices_count) {
}

MatrixGraph::MatrixGraph(const IGraph &graph): vertices_count_(graph.vertices_count()),
adjacency_matrix(vertices_count_ * vertices_count_) {
	for (unsigned int iter_vertex = 0; iter_vertex < vertices_count_; iter_vertex++) {
		for (const unsigned int &adjacent : graph.get_next_vertices(iter_vertex)) {
			adjacency_matrix[iter_vertex * vertices_count_ + adjacent] = true;
		}
	}
}

void MatrixGraph::add_edge(unsigned int from, unsigned int to) {
	adjacency_matrix[from * vertices_count() + to] = true;
}

size_t MatrixGraph::vertices_count() const {
	return vertices_count_;
}

std::vector<unsigned int> MatrixGraph::get_next_vertices(unsigned int vertex) const {
	std::vector<unsigned int> result;
	for (unsigned int i = 0; i < vertices_count(); i++) {
		if (adjacency_matrix[vertex * vertices_count() + i]) {
			result.push_back(i);
		}
	}
	return result;
}

std::vector<unsigned int> MatrixGraph::get_prev_vertices(unsigned int vertex) const {
	std::vector<unsigned int> result;
	for (unsigned int i = 0; i < vertices_count(); i++) {
		if (adjacency_matrix[i * vertices_count() + vertex]) {
			result.push_back(i);
		}
	}
	return result;
}

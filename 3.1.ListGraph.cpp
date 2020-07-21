#include "ListGraph.hpp"

ListGraph::ListGraph(size_t vertices_count): vertices(vertices_count) {
}

ListGraph::ListGraph(const IGraph &graph): vertices(graph.vertices_count()) {
	for (unsigned int iter_vertex = 0; iter_vertex < vertices.size(); iter_vertex++) {
		vertices[iter_vertex] = graph.get_next_vertices(iter_vertex);
	}
}

void ListGraph::add_edge(unsigned int from, unsigned int to) {
	vertices[from].push_back(to);
}

size_t ListGraph::vertices_count() const {
	return vertices.size();
}

std::vector<unsigned int> ListGraph::get_next_vertices(unsigned int vertex) const {
	return vertices[vertex]; //возвращается копия. доказано тестами
}

std::vector<unsigned int> ListGraph::get_prev_vertices(unsigned int vertex) const {
	std::vector<unsigned int> result;
	for (unsigned int iter_vertex = 0; iter_vertex < vertices_count(); iter_vertex++) {
		for (const unsigned int &adjacent_vertex : vertices[iter_vertex]) {
			if (adjacent_vertex == vertex) {
				result.push_back(iter_vertex);
			}
		}
	}
	return result;
}

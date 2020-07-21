#include <iostream>
#include <set>
#include <vector>

void run(std::istream&, std::ostream&);

class ListGraph {
private:
	std::vector<std::vector<std::pair<unsigned int, size_t>>> vertices;
public:
	explicit ListGraph(size_t);
	void add_edge(unsigned int, unsigned int, size_t);
	size_t vertices_count() const;
	std::vector<std::pair<unsigned int, size_t>> get_next_vertices(unsigned int) const;
	std::vector<size_t> dijkstra(unsigned int) const;
};

int main() {
	run(std::cin, std::cout);
	return 0;
}

void run(std::istream &in, std::ostream &out) {
	size_t n, m;
	in >> n >> m;
	ListGraph graph(n);
	unsigned int vertex1, vertex2;
	size_t time_;
	do {
		in >> vertex1 >> vertex2 >> time_;
		if (vertex1 != vertex2) {
			graph.add_edge(vertex1, vertex2, time_);
			graph.add_edge(vertex2, vertex1, time_);
		}
	} while (--m > 0);
	in >> vertex1 >> vertex2;
	auto shortest_distances = graph.dijkstra(vertex1);
	out << shortest_distances[vertex2];
}

ListGraph::ListGraph(size_t vertices_count): vertices(vertices_count) {
}

void ListGraph::add_edge(unsigned int from, unsigned int to, size_t length) {
	vertices[from].push_back(std::pair<unsigned int, size_t>(to, length));
}

size_t ListGraph::vertices_count() const {
	return vertices.size();
}

std::vector<std::pair<unsigned int, size_t>> ListGraph::get_next_vertices(unsigned int vertex) const {
	return vertices[vertex];
}

std::vector<size_t> ListGraph::dijkstra(unsigned int from) const {
	struct Vertex {
		unsigned int number;
		size_t shortest_distance;
		Vertex(): number(0), shortest_distance(-1) {
		}
		bool operator() (const Vertex &vertex1, const Vertex &vertex2) const {
			return vertex1.shortest_distance < vertex2.shortest_distance;
		}
	};
	auto dijkstra_vertices = new Vertex[vertices_count()];
	for (size_t index = vertices_count() - 1; index > 0; index--) {
		dijkstra_vertices[index].number = index;
	}
	dijkstra_vertices[from].shortest_distance = 0;
	std::set<Vertex*> pq;
	pq.insert(dijkstra_vertices + from);
	do {
		Vertex *current_vertex = *pq.begin();
		pq.erase(pq.begin()); //амортизированная константа
		for (auto adjacent : get_next_vertices(current_vertex->number)) {
			if (dijkstra_vertices[adjacent.first].shortest_distance == -1) {
				dijkstra_vertices[adjacent.first].shortest_distance = current_vertex->shortest_distance + adjacent.second;
				pq.insert(dijkstra_vertices + adjacent.first);
			} else if (dijkstra_vertices[adjacent.first].shortest_distance > current_vertex->shortest_distance + adjacent.second) {
				pq.erase(dijkstra_vertices + adjacent.first);
				dijkstra_vertices[adjacent.first].shortest_distance = current_vertex->shortest_distance + adjacent.second;
				pq.insert(dijkstra_vertices + adjacent.first);
			}
		}
	} while (!pq.empty());
	std::vector<size_t> result(vertices_count(), -1);
	size_t index = vertices_count();
	do {
		index--;
		result[index] = dijkstra_vertices[index].shortest_distance;
	} while (index > 0);
	delete[] dijkstra_vertices;
	return result;
}

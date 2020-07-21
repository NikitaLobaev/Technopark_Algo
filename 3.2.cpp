#include <iostream>
#include <queue>

void run(std::istream&, std::ostream&);

int main() {
	run(std::cin, std::cout);
	return 0;
}

class ListGraph {
private:
	std::vector<std::vector<unsigned int>> vertices;
public:
	explicit ListGraph(size_t);
	size_t vertices_count() const;
	void add_edge(unsigned int, unsigned int);
	std::vector<unsigned int> get_next_vertices(unsigned int) const;
	size_t find_shortest_paths_count(unsigned int, unsigned int) const;
};

void run(std::istream &in, std::ostream &out) {
	size_t v, n;
	in >> v >> n;
	ListGraph graph(v);
	unsigned int u, w;
	do {
		in >> u >> w;
		graph.add_edge(u, w);
		graph.add_edge(w, u);
	} while (--n > 0);
	in >> u >> w;
	out << graph.find_shortest_paths_count(u, w);
}

ListGraph::ListGraph(size_t vertices_count): vertices(vertices_count) {
}

void ListGraph::add_edge(unsigned int from, unsigned int to) {
	vertices[from].push_back(to);
}

size_t ListGraph::vertices_count() const {
	return vertices.size();
}

std::vector<unsigned int> ListGraph::get_next_vertices(unsigned int vertex) const {
	return vertices[vertex];
}

size_t ListGraph::find_shortest_paths_count(unsigned int from, unsigned int to) const {
	enum VertexColor { white, gray, black };
	struct Vertex {
		VertexColor color;
		size_t shortest_distance, shortest_paths_count;
		Vertex(): color(white), shortest_distance(-1), shortest_paths_count(0) {
		}
	};
	std::vector<Vertex> vertices_bfs(vertices_count());
	vertices_bfs[from].color = gray;
	vertices_bfs[from].shortest_distance = 0;
	vertices_bfs[from].shortest_paths_count = 1;
	std::queue<unsigned int> queue;
	queue.push(from);
	unsigned int current_vertex;
	do {
		current_vertex = queue.front();
		queue.pop();
		for (const unsigned int &next_vertex : get_next_vertices(current_vertex)) {
			if (vertices_bfs[next_vertex].color == white) {
				vertices_bfs[next_vertex].color = gray;
				vertices_bfs[next_vertex].shortest_distance = vertices_bfs[current_vertex].shortest_distance + 1;
				vertices_bfs[next_vertex].shortest_paths_count = vertices_bfs[current_vertex].shortest_paths_count;
				queue.push(next_vertex);
			} else if (vertices_bfs[next_vertex].color == gray && vertices_bfs[next_vertex].shortest_distance ==
			vertices_bfs[current_vertex].shortest_distance + 1) {
				vertices_bfs[next_vertex].shortest_paths_count += vertices_bfs[current_vertex].shortest_paths_count;
			} else if (vertices_bfs[next_vertex].color == black && next_vertex == to) {
				break; //алгоритм BFS гарантирует, что если так, то можно дальше не искать
			}
		}
		vertices_bfs[current_vertex].color = black;
	} while (!queue.empty());
	return vertices_bfs[to].shortest_paths_count;
}

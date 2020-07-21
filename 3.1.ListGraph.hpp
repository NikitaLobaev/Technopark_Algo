#ifndef HOMEWORK3_LISTGRAPH_HPP
#define HOMEWORK3_LISTGRAPH_HPP


#include <iostream>
#include "IGraph.hpp"

class ListGraph : public IGraph {
private:
	std::vector<std::vector<unsigned int>> vertices;
public:
	explicit ListGraph(size_t);
	explicit ListGraph(const IGraph &);
	void add_edge(unsigned int, unsigned int) override;
	size_t vertices_count() const override;
	std::vector<unsigned int> get_next_vertices(unsigned int) const override;
	std::vector<unsigned int> get_prev_vertices(unsigned int) const override;
};


#endif //HOMEWORK3_LISTGRAPH_HPP

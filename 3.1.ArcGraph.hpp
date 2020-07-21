#ifndef HOMEWORK3_ARCGRAPH_HPP
#define HOMEWORK3_ARCGRAPH_HPP


#include <iostream>
#include "IGraph.hpp"

class ArcGraph : public IGraph {
private:
	std::vector<std::pair<unsigned int, unsigned int>> edges;
	size_t vertices_count_;
public:
	explicit ArcGraph(size_t);
	explicit ArcGraph(const IGraph &);
	void add_edge(unsigned int, unsigned int) override;
	size_t vertices_count() const override;
	std::vector<unsigned int> get_next_vertices(unsigned int) const override;
	std::vector<unsigned int> get_prev_vertices(unsigned int) const override;
};


#endif //HOMEWORK3_ARCGRAPH_HPP

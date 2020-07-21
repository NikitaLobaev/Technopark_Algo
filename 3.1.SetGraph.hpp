#ifndef HOMEWORK3_SETGRAPH_HPP
#define HOMEWORK3_SETGRAPH_HPP


#include <iostream>
#include <unordered_set>
#include "IGraph.hpp"

class SetGraph : public IGraph {
private:
	std::vector<std::unordered_set<unsigned int>> sets;
public:
	explicit SetGraph(size_t);
	explicit SetGraph(const IGraph &);
	void add_edge(unsigned int, unsigned int) override;
	size_t vertices_count() const override;
	std::vector<unsigned int> get_next_vertices(unsigned int) const override;
	std::vector<unsigned int> get_prev_vertices(unsigned int) const override;
};


#endif //HOMEWORK3_SETGRAPH_HPP

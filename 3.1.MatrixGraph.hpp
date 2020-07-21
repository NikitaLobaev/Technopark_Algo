#ifndef HOMEWORK3_MATRIXGRAPH_HPP
#define HOMEWORK3_MATRIXGRAPH_HPP


#include <iostream>
#include "IGraph.hpp"

class MatrixGraph : public IGraph {
private:
	size_t vertices_count_;
	std::vector<bool> adjacency_matrix;
public:
	explicit MatrixGraph(size_t);
	explicit MatrixGraph(const IGraph &);
	void add_edge(unsigned int, unsigned int) override;
	size_t vertices_count() const override;
	std::vector<unsigned int> get_next_vertices(unsigned int) const override;
	std::vector<unsigned int> get_prev_vertices(unsigned int) const override;
};


#endif //HOMEWORK3_MATRIXGRAPH_HPP

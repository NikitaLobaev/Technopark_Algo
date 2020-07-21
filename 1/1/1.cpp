#include <iostream>

void run(std::istream&, std::ostream&);

int main() {
	run(std::cin, std::cout);
	return 0;
}

void get_max_sum(int*, int*, size_t, size_t&, size_t&);

void run(std::istream &in, std::ostream &out) {
	size_t n;
	in >> n;
	auto a = new int[n], b = new int[n], a_iter = a, b_iter = b, end = a + n;
	for (; a_iter < end; a_iter++) {
		in >> *a_iter;
	}
	for (end = b + n; b_iter < end; b_iter++) {
		in >> *b_iter;
	}
	size_t i0, j0;
	get_max_sum(a, b, n, i0, j0);
	out << i0 << ' ' << j0;
	delete[] a;
	delete[] b;
}

void get_max_sum(int *a, int *b, size_t n, size_t &i0, size_t &j0) {
	auto a_iter = a, b_iter = b, current_max_a = a_iter, pi0 = a_iter, pj0 = b_iter; //для оптимизации работы алгоритма
	int *const end = a + n;
	int current_max_sum = *current_max_a + *b_iter, current_sum;
	while (++a_iter < end) {
		b_iter++;
		current_sum = *current_max_a;
		if (current_sum < *a_iter) {
			current_max_a = a_iter;
			current_sum = *current_max_a + *b_iter;
		} else {
			current_sum += *b_iter;
		}
		if (current_max_sum < current_sum) {
			current_max_sum = current_sum;
			pi0 = current_max_a;
			pj0 = b_iter;
		}
	}
	i0 = pi0 - a; //так как вместо индексов массива, использовались указатели на соответствующие элементы
	j0 = pj0 - b;
}

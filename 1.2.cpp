#include <cmath>
#include <iostream>

#define min(a, b) ((a) > (b) ? (b) : (a))

void run(std::istream&, std::ostream&);

int main() {
	run(std::cin, std::cout);
	return 0;
}

size_t get_intersection(const int *a, const int *b, size_t n, size_t m, int *result);

void run(std::istream &in, std::ostream &out) {
	size_t n, m, index;
	in >> n >> m;
	auto a = new int[n], b = new int[m];
	for (index = 0; index < n; index++) {
		in >> a[index];
	}
	for (index = 0; index < m; index++) {
		in >> b[index];
	}
	auto intersection = new int[m]; //так как по условию n >> m (n намного больше m), размер пересечения <= m
	size_t length = get_intersection(a, b, n, m, intersection);
	for (index = 0; index < length; index++) {
		out << intersection[index] << ' ';
	}
	delete[] a;
	delete[] b;
}

size_t bin_search(const int*, int, size_t, size_t);

size_t get_intersection(const int *a, const int *b, size_t n, size_t m, int *result) { //функция возвращает размер пересечения
	size_t from = 0, to, arrays_index = 0, result_index = 0; //from, to - диапазон бинарного поиска [from, to)
	do {
		to = from + 1;
		while (to <= n && a[to - 1] < b[arrays_index]) {
			to *= 2;
		}
		from = bin_search(a, b[arrays_index], from, min(to, n));
		if (from < n && a[from] == b[arrays_index]) {
			result[result_index++] = a[from];
			from++;
		}
		arrays_index++;
	} while (--m > 0);
	return result_index;
}

size_t bin_search(const int *array, int element, size_t from, size_t to) { //отрезок [from, to). Возвращает индекс
	size_t middle;
	while (from < to) {
		middle = (from + to) / 2;
		if (array[middle] < element) {
			from = middle + 1;
		} else {
			to = middle;
		}
	}
	return from;
}

#include <cstdlib>
#include <ctime>
#include <iostream>

void run(std::istream&, std::ostream&);

int main() {
	unsigned int seed = time(nullptr);
	srand(seed);
	run(std::cin, std::cout);
	return 0;
}

size_t get_ord_stats(unsigned int*, size_t length, size_t k);

void run(std::istream &in, std::ostream &out) {
	size_t n, k, index;
	in >> n >> k;
	auto array = new unsigned int[n];
	for (index = 0; index < n; index++) {
		in >> array[index];
	}
	index = get_ord_stats(array, n, k); //индекс искомого элемента
	out << array[index];
	delete[] array;
}

bool compare_integers(const unsigned int&, const unsigned int&);

template <class Element>
size_t partition(Element*, size_t, size_t, bool (*)(const Element&, const Element&));

size_t get_ord_stats(unsigned int *array, size_t length, size_t k) {
	size_t from = 0, to = length - 1, index;
	while (true) {
		index = partition(array, from, to, compare_integers);
		if (index > k) {
			to = index - 1;
		} else if (index < k) {
			from = index + 1;
		} else {
			break;
		}
	}
	return index;
}

template <class Element>
size_t median_of_three(Element*, size_t, size_t, bool (*)(const Element&, const Element&));

//отрезок [start, end], НЕ интервал/полуинтервал!
template <class Element>
size_t partition(Element *array, size_t start, size_t end, bool (*compare)(const Element&, const Element&)) {
	if (start == end) {
		return start;
	}
	auto i = array + start, last = array + end;
	std::swap(array[median_of_three(array, start, end + 1 - start, compare)], *last); //поменять опорный с последним
	while (i < last && !compare(*last, *i)) {
		i++;
	}
	auto j = i;
	while (++j < last) {
		if (!compare(*last, *j)) {
			std::swap(*i, *j);
			i++;
		}
	}
	std::swap(*i, *last);
	return i - array; //приводится к size_t как разница указателей на Element (индекс опорного)
}

template <class Element>
size_t median_of_three(Element *array, size_t from, size_t length, bool (*compare)(const Element&, const Element&)) {
	size_t a = from + rand() % length, b = from + rand() % length, c = from + rand() % length;
	if (compare(array[a], array[c])) {
		if (compare(array[b], array[a])) {
			return a;
		} else if (compare(array[b], array[c])) {
			return c;
		} else {
			return b;
		}
	} else if (compare(array[b], array[c])) {
		return c;
	} else if (compare(array[a], array[b])) {
		return a;
	} else {
		return b;
	}
}

bool compare_integers(const unsigned int &a, const unsigned int &b) {
	return a < b;
}

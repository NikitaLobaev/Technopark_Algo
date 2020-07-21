#include <iostream>

void run(std::istream&, std::ostream&);

int main() {
	run(std::cin, std::cout);
	return 0;
}

template <class T>
void radix_sort(T *, size_t); //MSD. Работает только для unsigned типов!

void run(std::istream &in, std::ostream &out) {
	size_t n, index;
	in >> n;
	auto array = new unsigned long long[n];
	for (index = 0; index < n; index++) {
		in >> array[index];
	}
	radix_sort(array, n);
	for (index = 0; index < n; index++) {
		out << array[index] << ' ';
	}
	delete[] array;
}

template <class T>
void radix_sort_partition(T*, T*, unsigned char);

template <class T>
void radix_sort(T *array, size_t length) {
	if (length > 1) {
		radix_sort_partition(array, array + length - 1, 8 * sizeof(T) - 1);
	}
}

template <class T>
void radix_sort_partition(T *start, T *end, unsigned char bit) { //отрезок [start, end]
	auto i = start;
	while (i <= end && (*i >> bit & 1u) == 0) {
		i++;
	}
	auto j = i;
	while (++j <= end) {
		if ((*j >> bit & 1u) == 0) {
			std::swap(*i, *j);
			i++;
		}
	}
	if (bit-- != 0) {
		if (start + 1 < i) {
			radix_sort_partition(start, i - 1, bit);
		}
		if (i < end) {
			radix_sort_partition(i, end, bit);
		}
	}
}

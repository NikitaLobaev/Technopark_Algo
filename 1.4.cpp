#include <iostream>

void run(std::istream&, std::ostream&);

int main() {
	run(std::cin, std::cout);
	return 0;
}

void get_window_max(const int*, size_t, size_t, int*);

void run(std::istream &in, std::ostream &out) {
	size_t n, k, index;
	in >> n;
	auto array = new int[n];
	for (index = 0; index < n; index++) {
		in >> array[index];
	}
	in >> k;
	auto result = new int[n - k + 1];
	get_window_max(array, n, k, result);
	for (index = 0; index <= n - k; index++) {
		out << result[index] << ' ';
	}
	delete[] array;
	delete[] result;
}

template <class HeapElement>
class Heap { //куча с фиксированным размером буфера
private:
	HeapElement *buffer;
	size_t heap_size;
	bool (*comparator)(const HeapElement&, const HeapElement&);
	void sift_up(size_t);
	void sift_down(size_t);
public:
	Heap(const HeapElement*, size_t, size_t, bool (*)(const HeapElement&, const HeapElement&)); //создание кучи из массива
	Heap(const Heap&) = delete;
	Heap(Heap&&) = delete;
	Heap &operator= (const Heap&) = delete;
	Heap &operator= (Heap&&) = delete;
	~Heap();
	size_t get_size() const;
	bool is_empty() const;
	void push(const HeapElement&);
	const HeapElement &top() const;
	void pop();
};

void get_window_max(const int *array, size_t array_size, size_t window_size, int *result) {
	struct ArrayElement {
		size_t index;
		int value;
		bool operator()(const ArrayElement &l, const ArrayElement &r) {
			return l.value < r.value;
		}
	};
	size_t index;
	auto heap_elements = new ArrayElement[array_size];
	for (index = 0; index < array_size; index++) {
		heap_elements[index].index = index;
		heap_elements[index].value = array[index];
	}
	Heap<ArrayElement> heap(heap_elements, window_size, array_size, [](const ArrayElement &l,
			const ArrayElement &r) {
		return l.value < r.value;
	});
	index = 0;
	while (true) {
		while (heap.top().index < index) {
			heap.pop();
		}
		result[index] = heap.top().value;
		if (index == array_size - window_size) {
			break;
		}
		heap.push(heap_elements[index + window_size]);
		index++;
	}
}

template <class HeapElement>
Heap<HeapElement>::Heap(const HeapElement *array, size_t heap_size, size_t buffer_size,
		bool (*compare)(const HeapElement&, const HeapElement&)): heap_size(heap_size), comparator(compare) {
	buffer = new HeapElement[buffer_size];
	size_t index;
	for (index = 0; index < heap_size; index++) {
		buffer[index] = array[index];
	}
	index = heap_size / 2;
	while (index-- > 0) {
		sift_down(index);
	}
}

template <class HeapElement>
Heap<HeapElement>::~Heap() {
	delete[] buffer;
}

template <class HeapElement>
void Heap<HeapElement>::sift_up(size_t index) {
	while (index > 0) {
		size_t parent = (index - 1) / 2;
		if (!comparator(buffer[parent], buffer[index])) {
			return;
		}
		std::swap(buffer[index], buffer[parent]);
		index = parent;
	}
}

template <class HeapElement>
void Heap<HeapElement>::sift_down(size_t index) {
	size_t left = 2 * index + 1, right = 2 * index + 2, largest = index;
	if (left < get_size() && comparator(buffer[largest], buffer[left])) {
		largest = left;
	}
	if (right < get_size() && comparator(buffer[largest], buffer[right])) {
		largest = right;
	}
	if (largest != index) {
		std::swap(buffer[index], buffer[largest]);
		sift_down(largest);
	}
}

template <class HeapElement>
size_t Heap<HeapElement>::get_size() const {
	return heap_size;
}

template <class HeapElement>
bool Heap<HeapElement>::is_empty() const {
	return get_size() == 0;
}

template <class HeapElement>
void Heap<HeapElement>::push(const HeapElement &element) {
	buffer[heap_size] = element;
	sift_up(heap_size++);
}

template <class HeapElement>
const HeapElement &Heap<HeapElement>::top() const {
	return buffer[0];
}

template <class HeapElement>
void Heap<HeapElement>::pop() {
	buffer[0] = buffer[--heap_size];
	if (!is_empty()) {
		sift_down(0);
	}
}

#include <iostream>

void run(std::istream&, std::ostream&);

int main() {
	run(std::cin, std::cout);
	return 0;
}

template <class T> //общей реализации нет, так как в данной задаче не требуется
struct HashFunc;

template <> //реализация для строк в соответствии с требованием задачи
struct HashFunc<std::string> {
	size_t operator() (const std::string &key) {
		size_t hash = 0;
		for (const char *str = key.data(); *str != '\0'; str++) {
			hash = hash * 257 + *str; //не 256, т.к. должна быть взаимная простота с sizeof(size_t) и размером хеш-таблицы (а размер хеш-таблицы это степень двойки)
		}
		return hash;
	}
};

template <class T>
struct EqualsFunc;

template <>
struct EqualsFunc<std::string> {
	bool operator() (const std::string &a, const std::string &b) const {
		return a == b;
	}
};

template <class Key, class Hash = HashFunc<Key>, class Equals = EqualsFunc<Key>>
class HashSet {
	struct Element {
		Key key;
	};
	struct Node {
		Element element;
		bool empty, deleted;
		Node(): empty(true), deleted(false) {
		}
	};
private:
	Node *buckets;
	size_t buckets_count, items_count, del_count;
	Hash hash;
	Equals equals;
	void grow();
	Node *find_node(const Key &);
public:
	explicit HashSet(Hash hash = Hash(), Equals equals = Equals());
	~HashSet();
	bool insert(const Key &);
	bool find(const Key &);
	bool erase(const Key &);
};

void run(std::istream &in, std::ostream &out) {
	HashSet<std::string, HashFunc<std::string>, EqualsFunc<std::string>> hash_set{};
	char operation = '\0';
	std::string key;
	while (in >> operation >> key) {
		bool operation_result;
		switch (operation) {
			case '+':
				operation_result = hash_set.insert(key);
				break;
			case '-':
				operation_result = hash_set.erase(key);
				break;
			case '?':
				operation_result = hash_set.find(key);
				break;
			default:
				operation_result = false;
				break;
		}
		if (operation_result) {
			out << "OK" << std::endl;
		} else {
			out << "FAIL" << std::endl;
		}
	}
}

template <class Key, class Hash, class Equals>
HashSet<Key, Hash, Equals>::HashSet(Hash hash, Equals equals): buckets_count(8), hash(hash), equals(equals),
items_count(0), del_count(0) {
	buckets = new Node[buckets_count];
}

template <class Key, class Hash, class Equals>
HashSet<Key, Hash, Equals>::~HashSet() {
	delete[] buckets;
}

template <class Key, class Hash, class Equals>
void HashSet<Key, Hash, Equals>::grow() { //увеличение размера хеш-таблицы и перехеширование соответственно
	buckets_count *= 2;
	auto buckets_new = new Node[buckets_count];
	size_t items_inserted = 0;
	for (auto buckets_iter = buckets; items_inserted < items_count; buckets_iter++) {
		if (!buckets_iter->empty) {
			size_t h = hash(buckets_iter->element.key), index = h % buckets_count;
			for (size_t i = 0; !buckets_new[index].empty; i++) {
				h += i + 1;
				index = h % buckets_count;
			}
			buckets_new[index].element = buckets_iter->element; //копия
			buckets_new[index].empty = false;
			items_inserted++;
		}
	}
	delete[] buckets;
	buckets = buckets_new;
}

template <class Key, class Hash, class Equals>
typename HashSet<Key, Hash, Equals>::Node *HashSet<Key, Hash, Equals>::find_node(const Key &key) {
	size_t h = hash(key), index = h % buckets_count, i;
	for (i = 0; i < buckets_count && ((!buckets[index].empty && !equals(key, buckets[index].element.key)) || buckets[index].deleted); i++) {
		h += i + 1;
		index = h % buckets_count;
	}
	if (i < buckets_count && !buckets[index].empty && equals(key, buckets[index].element.key)) {
		return buckets + index;
	} else {
		return nullptr;
	}
}

template <class Key, class Hash, class Equals>
bool HashSet<Key, Hash, Equals>::insert(const Key &key) {
	if (4 * items_count >= 3 * buckets_count) { //если коэффициент заполнения >= 75%
		grow();
	} else if (4 * (items_count + del_count) >= 3 * buckets_count) { //если коэффициент СВОБОДНЫХ ячеек для вставки < 25%
		auto elements = new Element[items_count], elements_iter = elements;
		const auto elements_end = elements + items_count;
		auto buckets_iter = buckets;
		const auto buckets_end = buckets + buckets_count;
		while (elements_iter < elements_end) {
			if (buckets_iter->empty) {
				buckets_iter->deleted = false;
			} else {
				*elements_iter++ = buckets_iter->element;
				buckets_iter->element = Element(); //очистка памяти, так как ячейка потом перемещается в другое место
				buckets_iter->empty = true;
			}
			buckets_iter++;
		}
		while (buckets_iter < buckets_end) {
			buckets_iter->deleted = false;
			buckets_iter++;
		}
		del_count = 0;
		for (elements_iter = elements; elements_iter < elements_end; elements_iter++) { //перехеширование
			const Key &current_key = elements_iter->key;
			size_t h = hash(current_key), index = h % buckets_count;
			for (size_t i = 0; !buckets[index].empty; i++) {
				h += i + 1;
				index = h % buckets_count;
			}
			buckets[index].element = *elements_iter;
			buckets[index].empty = false;
		}
		delete[] elements;
	}
	size_t h = hash(key), index = h % buckets_count;
	for (size_t i = 0; buckets[index].deleted || (!buckets[index].empty && !equals(key, buckets[index].element.key)); i++) {
		h += i + 1;
		index = h % buckets_count;
	}
	if (!buckets[index].empty) {
		return false;
	}
	buckets[index].element.key = key;
	buckets[index].empty = false;
	if (buckets[index].deleted) {
		buckets[index].deleted = false;
		del_count--;
	}
	items_count++;
	return true;
}

template <class Key, class Hash, class Equals>
bool HashSet<Key, Hash, Equals>::find(const Key &key) {
	return find_node(key) != nullptr;
}

template <class Key, class Hash, class Equals>
bool HashSet<Key, Hash, Equals>::erase(const Key &key) { //возвращает true <=> элемент существовал и был удалён
	auto node = find_node(key);
	if (node == nullptr) {
		return false;
	}
	node->element = Element();
	node->empty = node->deleted = true;
	items_count--;
	del_count++;
	return true;
}

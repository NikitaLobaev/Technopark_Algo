#include <iostream>

void run(std::istream &, std::ostream &);

int main() {
	run(std::cin, std::cout);
	return 0;
}

template <class Key>
struct KeyComparator {
	int operator() (const Key &a, const Key &b) {
		return a < b ? -1 : a > b;
	}
};

template <class Key, class KeyComparator = KeyComparator<Key>>
class AVLTree {
	struct Node {
		Key key;
		Node *left, *right;
		int8_t h_diff;
		size_t size;
		explicit Node(const Key&);
	};
private:
	KeyComparator key_comparator;
	Node *root;
	Node *insert_node(Node*, const Key&);
	Node *erase_node(Node*, const Key&);
	Node *rotate_left(Node*);
	Node *rotate_right(Node*);
	Node *balance(Node*);
	Node *remove_min_node(Node*, Node*&);
	int8_t get_node_h_diff(Node*) const;
	void fix_node_fields(Node*);
	size_t get_node_size(Node*) const;
	void delete_node(Node*);
public:
	explicit AVLTree(KeyComparator key_comparator = KeyComparator());
	~AVLTree();
	void insert(const Key &);
	Key find_ord_stats(size_t) const;
	void erase(const Key &);
};

void run(std::istream &in, std::ostream &out) {
	size_t n, k;
	in >> n;
	AVLTree<unsigned int> avl_tree;
	int a;
	while (n-- > 0) {
		in >> a >> k;
		if (a > 0) {
			avl_tree.insert((unsigned int) a);
		} else {
			avl_tree.erase((unsigned int) -a);
		}
		out << avl_tree.find_ord_stats(k) << std::endl;
	}
}

template<class Key, class KeyComparator>
AVLTree<Key, KeyComparator>::Node::Node(const Key &key): key(key), left(nullptr), right(nullptr), h_diff(1), size(1) {
}

template<class Key, class KeyComparator>
AVLTree<Key, KeyComparator>::AVLTree(KeyComparator key_comparator): key_comparator(key_comparator), root(nullptr) {
}

template<class Key, class KeyComparator>
void AVLTree<Key, KeyComparator>::insert(const Key &key) {
	root = insert_node(root, key);
}

template<class Key, class KeyComparator>
Key AVLTree<Key, KeyComparator>::find_ord_stats(size_t index) const { //корректность значения index не проверяется
	auto node = root;
	size_t from = 0, to = node->size; //поиск в полуинтервале [from; to)
	size_t node_left_size = get_node_size(node->left);
	while (index != from + node_left_size) {
		if (index < from + node_left_size) { //поиск в левом поддереве current_node
			to -= get_node_size(node->right) + 1;
			node = node->left;
		} else { //поиск в правом поддереве current_node
			from += node_left_size + 1;
			node = node->right;
		}
		node_left_size = get_node_size(node->left);
	}
	return node->key;
}

template<class Key, class KeyComparator>
void AVLTree<Key, KeyComparator>::erase(const Key &key) {
	root = erase_node(root, key);
}

template<class Key, class KeyComparator>
typename AVLTree<Key, KeyComparator>::Node *AVLTree<Key, KeyComparator>::insert_node(Node *node, const Key &key) {
	if (node == nullptr) {
		return new Node(key);
	}
	switch (key_comparator(key, node->key)) {
		case -1:
			node->left = insert_node(node->left, key);
			break;
		case 1:
			node->right = insert_node(node->right, key);
			break;
		default:
			return root;
	}
	return balance(node);
}

template<class Key, class KeyComparator>
typename AVLTree<Key, KeyComparator>::Node *AVLTree<Key, KeyComparator>::erase_node(Node *node, const Key &key) {
	if (node == nullptr) {
		return nullptr;
	}
	switch (key_comparator(key, node->key)) {
		case -1:
			node->left = erase_node(node->left, key);
			break;
		case 1:
			node->right = erase_node(node->right, key);
			break;
		default: //case 0: (equals)
			auto left = node->left, right = node->right;
			delete node;
			if (right == nullptr) {
				return left;
			}
			Node *right_min; //здесь будет указатель на минимальный узел правого поддерева удалённого узла node
			right = remove_min_node(right, right_min); //передаём указатель на правое поддерево узла node и ссылку
			right_min->left = left;
			right_min->right = right;
			return balance(right_min);
	}
	return balance(node);
}

template<class Key, class KeyComparator>
typename AVLTree<Key, KeyComparator>::Node *AVLTree<Key, KeyComparator>::balance(Node *node) {
	fix_node_fields(node);
	int8_t balance_factor = get_node_h_diff(node->right) - get_node_h_diff(node->left);
	if (balance_factor == 2) {
		if (get_node_h_diff(node->right->right) < get_node_h_diff(node->right->left)) {
			node->right = rotate_right(node->right);
		}
		return rotate_left(node);
	} else if (balance_factor == -2) {
		if (get_node_h_diff(node->left->left) < get_node_h_diff(node->left->right)) {
			node->left = rotate_left(node->left);
		}
		return rotate_right(node);
	} else {
		return node;
	}
}

template<class Key, class KeyComparator>
typename AVLTree<Key, KeyComparator>::Node *AVLTree<Key, KeyComparator>::rotate_left(Node *node) { //малый левый поворот
	auto right = node->right;
	node->right = right->left;
	right->left = node;
	fix_node_fields(node);
	fix_node_fields(right);
	return right;
}

template<class Key, class KeyComparator>
typename AVLTree<Key, KeyComparator>::Node *AVLTree<Key, KeyComparator>::rotate_right(Node *node) { //малый правый поворот
	auto left = node->left;
	node->left = left->right;
	left->right = node;
	fix_node_fields(node);
	fix_node_fields(left);
	return left;
}

template<class Key, class KeyComparator>
void AVLTree<Key, KeyComparator>::fix_node_fields(Node *node) {
	node->h_diff = 1 + std::max(get_node_h_diff(node->left), get_node_h_diff(node->right));
	node->size = 1 + get_node_size(node->left) + get_node_size(node->right);
}

template<class Key, class KeyComparator>
int8_t AVLTree<Key, KeyComparator>::get_node_h_diff(Node *node) const {
	if (node == nullptr) {
		return 0;
	}
	return node->h_diff;
}

template<class Key, class KeyComparator>
size_t AVLTree<Key, KeyComparator>::get_node_size(Node *node) const {
	if (node == nullptr) {
		return 0;
	}
	return node->size;
}

template<class Key, class KeyComparator>
AVLTree<Key, KeyComparator>::~AVLTree() {
	delete_node(root);
}

template<class Key, class KeyComparator>
void AVLTree<Key, KeyComparator>::delete_node(Node *node) {
	if (node != nullptr) {
		delete_node(node->left);
		delete_node(node->right);
		delete node;
	}
}

template<class Key, class KeyComparator>
typename AVLTree<Key, KeyComparator>::Node *AVLTree<Key, KeyComparator>::remove_min_node(Node *node, Node *&min) {
	if (node->left == nullptr) {
		min = node; //запоминаем минимальный узел текущего поддерева
		return node->right;
	}
	node->left = remove_min_node(node->left, min);
	return balance(node);
}

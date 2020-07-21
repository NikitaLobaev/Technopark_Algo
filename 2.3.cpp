#include <iostream>
#include <stack>

#define max(a, b) ((a) > (b) ? (a) : (b))

void run(std::istream&, std::ostream&);

int main() {
	run(std::cin, std::cout);
	return 0;
}

template <class Key, class Priority, class KeyComparator = std::less<Key>, class PriorityComparator = std::less<Priority>>
class Treap {
	struct Node {
		const Key key;
		const Priority priority;
		Node *left, *right;
		Node(const Key&, const Priority&);
	};
private:
	KeyComparator key_comparator;
	PriorityComparator priority_comparator;
	Node *root;
	void split(Node*, const Key&, Node*&, Node*&);
	size_t get_node_depth(Node*) const;
	void delete_node(Node*);
public:
	explicit Treap(KeyComparator key_comparator = KeyComparator(), PriorityComparator priority_comparator =
			PriorityComparator());
	~Treap();
	void insert(const Key&, const Priority&);
	size_t get_depth() const;
};

template <class Element, class ElementsComparator = std::less<Element>>
class BinaryTree {
	struct Node {
		const Element element;
		Node *left, *right;
		explicit Node(const Element&);
	};
private:
	ElementsComparator elements_comparator;
	Node *root;
	size_t depth;
public:
	explicit BinaryTree(ElementsComparator elements_comparator = ElementsComparator());
	~BinaryTree();
	void insert(const Element&);
	size_t get_depth() const;
};

void run(std::istream &in, std::ostream &out) {
	size_t n;
	in >> n;
	BinaryTree<int> binary_tree;
	Treap<int, int> treap;
	int x, y;
	while (n-- > 0) {
		in >> x >> y;
		treap.insert(x, y);
		binary_tree.insert(x); //несмотря на то, что передаётся ссылка, в узлах дерева всё-равно хранится копия
	}
	size_t depth1 = treap.get_depth(), depth2 = binary_tree.get_depth();
	if (depth1 > depth2) {
		out << depth1 - depth2;
	} else {
		out << depth2 - depth1;
	}
}

template<class Key, class Priority, class KeyComparator, class PriorityComparator>
Treap<Key, Priority, KeyComparator, PriorityComparator>::Node::Node(const Key &key, const Priority &priority): key(key),
priority(priority), left(nullptr), right(nullptr) {
}

template<class Key, class Priority, class KeyComparator, class PriorityComparator>
void Treap<Key, Priority, KeyComparator, PriorityComparator>::split(Node *node, const Key &key, Node *&left, Node *&right) {
	if (node == nullptr) {
		left = right = nullptr;
	} else if (key_comparator(key, node->key)) {
		split(node->left, key, left, node->left);
		right = node;
	} else {
		split(node->right, key, node->right, right);
		left = node;
	}
}

template<class Key, class Priority, class KeyComparator, class PriorityComparator>
size_t Treap<Key, Priority, KeyComparator, PriorityComparator>::get_node_depth(Node *node) const {
	if (node == nullptr) {
		return 0;
	}
	size_t left_depth = get_node_depth(node->left), right_depth = get_node_depth(node->right);
	return 1 + max(left_depth, right_depth);
}

template<class Key, class Priority, class KeyComparator, class PriorityComparator>
void Treap<Key, Priority, KeyComparator, PriorityComparator>::delete_node(Treap::Node *node) {
	if (node != nullptr) { //рекурсия разрешается, поскольку в среднем декартово дерево сбалансировано
		delete_node(node->left);
		delete_node(node->right);
		delete node;
	}
}

template<class Key, class Priority, class KeyComparator, class PriorityComparator>
Treap<Key, Priority, KeyComparator, PriorityComparator>::Treap(KeyComparator key_comparator,
		PriorityComparator priority_comparator): key_comparator(key_comparator),
		priority_comparator(priority_comparator), root(nullptr) {
}

template<class Key, class Priority, class KeyComparator, class PriorityComparator>
Treap<Key, Priority, KeyComparator, PriorityComparator>::~Treap() {
	delete_node(root);
}

template<class Key, class Priority, class KeyComparator, class PriorityComparator>
void Treap<Key, Priority, KeyComparator, PriorityComparator>::insert(const Key &key, const Priority &priority) {
	Node *parent_node = nullptr, *current_node = root;
	while (current_node != nullptr && !priority_comparator(current_node->priority, priority)) {
		parent_node = current_node;
		if (key_comparator(current_node->key, key)) {
			current_node = current_node->right;
		} else {
			current_node = current_node->left;
		}
	}
	auto new_node = new Node(key, priority);
	split(current_node, key, new_node->left, new_node->right);
	if (parent_node == nullptr) {
		root = new_node;
	} else if (key_comparator(key, parent_node->key)) {
		parent_node->left = new_node;
	} else {
		parent_node->right = new_node;
	}
}

template<class Key, class Priority, class KeyComparator, class PriorityComparator>
size_t Treap<Key, Priority, KeyComparator, PriorityComparator>::get_depth() const {
	return get_node_depth(root);
}

template<class Element, class ElementsComparator>
BinaryTree<Element, ElementsComparator>::Node::Node(const Element &element): element(element), left(nullptr),
right(nullptr) {
}

template <class Element, class ElementsComparator>
BinaryTree<Element, ElementsComparator>::BinaryTree(ElementsComparator elements_comparator):
elements_comparator(elements_comparator), root(nullptr), depth(0) {
}

template <class Element, class ElementsComparator>
BinaryTree<Element, ElementsComparator>::~BinaryTree() {
	if (root == nullptr) {
		return;
	}
	std::stack<Node*> stack;
	stack.push(root);
	do {
		auto current_node = stack.top();
		stack.pop();
		if (current_node->left != nullptr) {
			stack.push(current_node->left);
		}
		if (current_node->right != nullptr) {
			stack.push(current_node->right);
		}
		delete current_node;
	} while (!stack.empty());
}

template <class Element, class ElementsComparator>
void BinaryTree<Element, ElementsComparator>::insert(const Element &element) {
	auto new_node = new Node(element);
	if (root == nullptr) {
		root = new_node;
		depth = 1;
		return;
	}
	auto current_node = root;
	size_t current_depth = 2;
	while (true) {
		if (elements_comparator(element, current_node->element)) {
			if (current_node->left == nullptr) {
				current_node->left = new_node;
				break;
			}
			current_node = current_node->left;
		} else {
			if (current_node->right == nullptr) {
				current_node->right = new_node;
				break;
			}
			current_node = current_node->right;
		}
		current_depth++;
	}
	depth = max(depth, current_depth);
}

template<class Element, class ElementsComparator>
size_t BinaryTree<Element, ElementsComparator>::get_depth() const {
	return depth;
}

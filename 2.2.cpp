#include <functional>
#include <iostream>
#include <stack>

void run(std::istream&, std::ostream&);

int main() {
	run(std::cin, std::cout);
	return 0;
}

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
public:
	explicit BinaryTree(ElementsComparator elements_comparator = ElementsComparator());
	~BinaryTree();
	void insert(const Element&);
	void post_order(std::function<void(Element)>) const;
};

void run(std::istream &in, std::ostream &out) {
	BinaryTree<int> binary_tree;
	unsigned int n;
	in >> n;
	int i;
	while (n-- > 0) {
		in >> i;
		binary_tree.insert(i); //несмотря на то, что передаётся ссылка, в узлах дерева всё-равно хранится копия
	}
	binary_tree.post_order([&out](int element) {
		out << element << " ";
	});
}

template<class Element, class ElementsComparator>
BinaryTree<Element, ElementsComparator>::Node::Node(const Element &element): element(element), left(nullptr), right(nullptr) {
}

template <class Element, class ElementsComparator>
BinaryTree<Element, ElementsComparator>::BinaryTree(ElementsComparator elements_comparator):
elements_comparator(elements_comparator), root(nullptr) {
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
		return;
	}
	auto current_node = root;
	while (true) {
		if (elements_comparator(element, current_node->element)) {
			if (current_node->left == nullptr) {
				current_node->left = new_node;
				return;
			}
			current_node = current_node->left;
		} else {
			if (current_node->right == nullptr) {
				current_node->right = new_node;
				return;
			}
			current_node = current_node->right;
		}
	}
}

template <class Element, class ElementsComparator>
void BinaryTree<Element, ElementsComparator>::post_order(std::function<void(Element)> function) const {
	if (root == nullptr) {
		return;
	}
	std::stack<Node*> stack1, stack2;
	stack1.push(root);
	do {
		auto current_node = stack1.top();
		stack1.pop();
		stack2.push(current_node);
		if (current_node->left != nullptr) {
			stack1.push(current_node->left);
		}
		if (current_node->right != nullptr) {
			stack1.push(current_node->right);
		}
	} while (!stack1.empty());
	do {
		function(stack2.top()->element);
		stack2.pop();
	} while (!stack2.empty());
}

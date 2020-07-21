#include <iostream>

void run(std::istream&, std::ostream&);

int main() {
	run(std::cin, std::cout);
	return 0;
}

template <class StackElement>
class Stack {
private:
	StackElement *buffer;
	size_t stack_size, buffer_size;
public:
	Stack();
	Stack(const Stack&) = delete;
	Stack(Stack&&) = delete;
	Stack &operator= (const Stack&) = delete;
	Stack &operator= (Stack&&) = delete;
	~Stack();
	void push(const StackElement&);
	StackElement &pop();
	bool is_empty() const;
};

template <class QueueElement>
class Queue {
private:
	Stack<QueueElement> stack1, stack2;
public:
	Queue() = default;
	Queue(const Queue&) = delete;
	Queue(Queue&&) = delete;
	Queue &operator= (const Queue&) = delete;
	Queue &operator= (Queue&&) = delete;
	void push(const QueueElement&);
	QueueElement &pop();
	bool is_empty() const;
};

void run(std::istream &in, std::ostream &out) {
	size_t n;
	in >> n;
	Queue<unsigned int> queue;
	while (n-- > 0) {
		unsigned int command = 0, expected_value = 0, value;
		in >> command >> expected_value;
		switch (command) {
			case 2: //pop
				if (queue.is_empty()) {
					value = -1;
				} else {
					value = queue.pop();
				}
				if (value != expected_value) {
					out << "NO";
					return;
				}
				break;
			case 3: //push
				queue.push(expected_value);
				break;
			default:
				out << "NO";
				return;
		}
	}
	out << "YES";
}

template <class QueueElement>
void Queue<QueueElement>::push(const QueueElement &value) {
	stack1.push(value);
}

template <class QueueElement>
QueueElement &Queue<QueueElement>::pop() {
	if (stack2.is_empty()) {
		do {
			stack2.push(stack1.pop());
		} while (!stack1.is_empty());
	}
	return stack2.pop();
}

template <class QueueElement>
bool Queue<QueueElement>::is_empty() const {
	return stack1.is_empty() && stack2.is_empty();
}

template <class StackElement>
Stack<StackElement>::Stack(): buffer(nullptr), stack_size(0), buffer_size(0) {
}

template <class StackElement>
Stack<StackElement>::~Stack() {
	delete[] buffer;
}

template <class StackElement>
void Stack<StackElement>::push(const StackElement &value) {
	if (buffer_size == 0) {
		buffer = new StackElement[1];
		buffer_size = 1;
	} else if (stack_size == buffer_size) {
		auto new_buffer = new StackElement[buffer_size * 2];
		for (size_t index = 0; index < buffer_size; index++) {
			new_buffer[index] = buffer[index];
		}
		delete[] buffer;
		buffer = new_buffer;
		buffer_size *= 2;
	}
	buffer[stack_size++] = value;
}

template <class StackElement>
StackElement &Stack<StackElement>::pop() {
	return buffer[--stack_size];
}

template <class StackElement>
bool Stack<StackElement>::is_empty() const {
	return stack_size == 0;
}

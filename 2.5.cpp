#include <queue>
#include <stack>
#include "Huffman.h"

class BitInputStream {
private:
	IInputStream &input_stream;
	byte current_byte;
	uint8_t bit_pos;
public:
	explicit BitInputStream(IInputStream&);
	bool read_bit(byte&);
	bool read_byte(byte&);
};

class BitOutputStream {
private:
	IOutputStream &output_stream;
	byte current_byte;
	uint8_t bit_pos;
public:
	explicit BitOutputStream(IOutputStream&);
	void write_bit(byte);
	void write_byte(byte);
};

struct EncodedHuffmanByte {
	byte b;
	size_t frequency, code, code_length;
	EncodedHuffmanByte *left, *right;
	EncodedHuffmanByte();
	EncodedHuffmanByte(EncodedHuffmanByte*, EncodedHuffmanByte*);
};

struct DecodedHuffmanByte {
	byte b;
	DecodedHuffmanByte *left, *right;
	explicit DecodedHuffmanByte(byte&);
	DecodedHuffmanByte(DecodedHuffmanByte*, DecodedHuffmanByte*);
};

EncodedHuffmanByte *merge_ehb(EncodedHuffmanByte*, EncodedHuffmanByte*); //ehb = Encoded Huffman Byte

size_t fix_and_calc(EncodedHuffmanByte*, size_t, size_t); //вычисляет коды и стоимость узлов дерева

void print_huffman_tree(EncodedHuffmanByte*, uint16_t&, BitOutputStream&);

void delete_huffman_tree(EncodedHuffmanByte*); //удаляет дерево неполностью, только все нелистовые узлы

DecodedHuffmanByte *read_huffman_tree(uint16_t, BitInputStream&);

void delete_huffman_tree_full(DecodedHuffmanByte*);

void Encode(IInputStream &original, IOutputStream &compressed) {
	std::vector<byte> data;
	auto ehb = new EncodedHuffmanByte[256];
	uint16_t used_bytes_count = 0;
	byte current_byte;
	while (original.Read(current_byte)) {
		data.push_back(current_byte);
		if (ehb[current_byte].frequency++ == 0) {
			used_bytes_count++;
		}
	}
	if (used_bytes_count == 0) { //если данных нет, то нечего кодировать
		return;
	}
	auto compare_bytes = [](EncodedHuffmanByte *ehb1, EncodedHuffmanByte *ehb2) {
		return ehb1->frequency > ehb2->frequency;
	};
	std::priority_queue<EncodedHuffmanByte*, std::vector<EncodedHuffmanByte*>, decltype(compare_bytes)> used_bytes(compare_bytes);
	current_byte = 0;
	EncodedHuffmanByte *current_ehb = ehb, *left_ehb, *right_ehb;
	do {
		if (current_ehb->frequency > 0) {
			current_ehb->b = current_byte;
			used_bytes.push(current_ehb);
		}
		current_ehb++;
	} while (++current_byte != 0);
	while (used_bytes.size() > 1) { //построение дерева Хаффмана
		left_ehb = used_bytes.top();
		used_bytes.pop();
		right_ehb = used_bytes.top();
		used_bytes.pop();
		used_bytes.push(merge_ehb(left_ehb, right_ehb));
	}
	size_t estimated_length = 12 + fix_and_calc(used_bytes.top(), 0, 0); //размер оценивается в битах
	for (current_ehb = used_bytes.top(); current_ehb->right; current_ehb = current_ehb->right) { //вычисляем глубину самого правого элемента дерева Хаффмана
		estimated_length--;
	}
	size_t unused_bits_count = (8 - estimated_length) % 8; //"estimated_length % 8" не требуется, поскольку (8 * sizeof(size_t)) бит кратно 8 битам
	if ((estimated_length + unused_bits_count) / 8 > data.size()) { //если кодирование неэффективно
		compressed.Write(0);
		for (const byte &b : data) {
			compressed.Write(b);
		}
		return;
	}
	BitOutputStream bit_output_stream(compressed);
	bit_output_stream.write_bit(1);
	bit_output_stream.write_bit(unused_bits_count & 1u);
	bit_output_stream.write_bit((unused_bits_count >> 1u) & 1u);
	bit_output_stream.write_bit((unused_bits_count >> 2u) & 1u);
	bit_output_stream.write_byte((byte) (used_bytes_count - 1));
	print_huffman_tree(used_bytes.top(), used_bytes_count, bit_output_stream);
	size_t printing_bit_index;
	for (const byte &b : data) {
		const EncodedHuffmanByte &huffman_byte = ehb[b];
		for (printing_bit_index = 0; printing_bit_index < huffman_byte.code_length; printing_bit_index++) {
			bit_output_stream.write_bit((huffman_byte.code >> printing_bit_index) & 1u);
		}
	}
	while (unused_bits_count-- > 0) { //дописываем в конец последнего байта неиспользуемые биты (нули)
		bit_output_stream.write_bit(0);
	}
	delete_huffman_tree(used_bytes.top());
	delete[] ehb; //поскольку листовые узлы дерева были выделены отдельно
}

void Decode(IInputStream &compressed, IOutputStream &original) {
	BitInputStream bit_input_stream(compressed);
	byte current_bit, current_byte;
	if (!bit_input_stream.read_bit(current_bit)) { //если данных нет, то нечего раскодировать
		return;
	}
	if (!current_bit) {
		for (uint8_t pass = 1; pass < 8; pass++) {
			bit_input_stream.read_bit(current_bit);
		}
		while (bit_input_stream.read_byte(current_byte)) {
			original.Write(current_byte);
		}
		return;
	}
	bit_input_stream.read_bit(current_bit);
	size_t unused_bits_count = current_bit;
	bit_input_stream.read_bit(current_bit);
	if (current_bit) {
		unused_bits_count += 2;
	}
	bit_input_stream.read_bit(current_bit);
	if (current_bit) {
		unused_bits_count += 4;
	}
	bit_input_stream.read_byte(current_byte);
	uint16_t used_bytes_count = (uint16_t) current_byte + 1;
	auto huffman_tree_root = read_huffman_tree(used_bytes_count, bit_input_stream); //построение дерева Хаффмана
	auto current_node = huffman_tree_root;
	std::deque<byte> deque;
	uint8_t depth = 0;
	while (depth < 8 && current_node->left) {
		depth++;
		current_node = current_node->left;
	}
	uint8_t wrong_bytes_count;
	if (depth < 2) {
		wrong_bytes_count = unused_bits_count;
	} else {
		wrong_bytes_count = unused_bits_count / depth;
	}
	current_node = huffman_tree_root;
	if (used_bytes_count > 1) {
		while (bit_input_stream.read_bit(current_bit)) {
			if (current_bit) {
				current_node = current_node->right;
			} else {
				current_node = current_node->left;
			}
			if (!current_node->left) {
				deque.push_back(current_node->b);
				if (deque.size() > wrong_bytes_count) {
					original.Write(deque.front());
					deque.pop_front();
				}
				current_node = huffman_tree_root;
			}
		}
	} else {
		while (bit_input_stream.read_bit(current_bit)) {
			deque.push_back(current_node->b);
			if (deque.size() > wrong_bytes_count) {
				original.Write(deque.front());
				deque.pop_front();
			}
			current_node = huffman_tree_root;
		}
	}
	delete_huffman_tree_full(huffman_tree_root);
}

void print_huffman_tree(EncodedHuffmanByte *current_ehb, uint16_t &remaining_bytes_count,
		BitOutputStream &bit_output_stream) {
	if (current_ehb->left) {
		print_huffman_tree(current_ehb->left, remaining_bytes_count, bit_output_stream);
		print_huffman_tree(current_ehb->right, remaining_bytes_count, bit_output_stream);
		if (remaining_bytes_count > 0) {
			bit_output_stream.write_bit(0);
		}
	} else {
		bit_output_stream.write_bit(1);
		bit_output_stream.write_byte(current_ehb->b);
		remaining_bytes_count--;
	}
}

EncodedHuffmanByte *merge_ehb(EncodedHuffmanByte *left_ehb, EncodedHuffmanByte *right_ehb) {
	return new EncodedHuffmanByte(left_ehb, right_ehb);
}

size_t fix_and_calc(EncodedHuffmanByte *current_ehb, size_t code, size_t bits_count) {
	if (!current_ehb->left) {
		current_ehb->code = code;
		if (bits_count == 0) { //если в дереве Хаффмана всего один узел
			bits_count = 1;
		}
		current_ehb->code_length = bits_count;
		return bits_count * current_ehb->frequency + 9;
	}
	return fix_and_calc(current_ehb->left, code, bits_count + 1) +
			fix_and_calc(current_ehb->right, code | (1u << bits_count), bits_count + 1) + 1;
}

void delete_huffman_tree(EncodedHuffmanByte *current_ehb) {
	if (current_ehb->left) {
		delete_huffman_tree(current_ehb->left); //рекурсия допустима, поскольку глубина дерева не больше 11
		delete_huffman_tree(current_ehb->right);
		delete current_ehb;
	}
}

DecodedHuffmanByte *read_huffman_tree(uint16_t used_bytes_count, BitInputStream &bit_input_stream) {
	std::stack<DecodedHuffmanByte*> stack;
	byte b;
	DecodedHuffmanByte *left, *right;
	while (used_bytes_count > 0) {
		bit_input_stream.read_bit(b);
		if (b) {
			bit_input_stream.read_byte(b);
			stack.push(new DecodedHuffmanByte(b));
			used_bytes_count--;
		} else {
			right = stack.top();
			stack.pop();
			left = stack.top();
			stack.pop();
			stack.push(new DecodedHuffmanByte(left, right));
		}
	}
	while (stack.size() > 1) {
		right = stack.top();
		stack.pop();
		left = stack.top();
		stack.pop();
		stack.push(new DecodedHuffmanByte(left, right));
	}
	return stack.top();
}

void delete_huffman_tree_full(DecodedHuffmanByte *current_dhb) {
	if (current_dhb->left) {
		delete_huffman_tree_full(current_dhb->left);
		delete_huffman_tree_full(current_dhb->right);
	}
	delete current_dhb;
}

BitInputStream::BitInputStream(IInputStream &input_stream): input_stream(input_stream), current_byte(0), bit_pos(0) {
}

bool BitInputStream::read_bit(byte &b) {
	if (bit_pos == 0 && !input_stream.Read(current_byte)) {
		return false;
	}
	b = (current_byte >> bit_pos) & 1u;
	if (bit_pos++ == 7) {
		bit_pos = 0;
	}
	return true;
}

bool BitInputStream::read_byte(byte &b) {
	if (bit_pos == 0) {
		return input_stream.Read(b);
	}
	b = current_byte >> bit_pos;
	if (!input_stream.Read(current_byte)) {
		return false;
	}
	b |= current_byte << (8u - bit_pos);
	return true;
}

BitOutputStream::BitOutputStream(IOutputStream &output_stream): output_stream(output_stream), current_byte(0), bit_pos(0) {
}

void BitOutputStream::write_bit(byte bit) {
	if (bit) {
		current_byte |= 1u << bit_pos;
	}
	if (++bit_pos == 8) {
		output_stream.Write(current_byte);
		current_byte = 0;
		bit_pos = 0;
	}
}

void BitOutputStream::write_byte(byte b) {
	output_stream.Write(current_byte | (b << bit_pos));
	current_byte = b >> (8u - bit_pos);
}

EncodedHuffmanByte::EncodedHuffmanByte(): b(0), frequency(0), code(0), code_length(0), left(nullptr), right(nullptr) {
}

EncodedHuffmanByte::EncodedHuffmanByte(EncodedHuffmanByte *left_ehb, EncodedHuffmanByte *right_ehb): b(0),
frequency(left_ehb->frequency + right_ehb->frequency), code(0), code_length(0), left(left_ehb), right(right_ehb) {
}

DecodedHuffmanByte::DecodedHuffmanByte(byte &b): b(b), left(nullptr), right(nullptr) {
}

DecodedHuffmanByte::DecodedHuffmanByte(DecodedHuffmanByte *left_dhb, DecodedHuffmanByte *right_dhb): b(0),
left(left_dhb), right(right_dhb) {
}

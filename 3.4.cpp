#include <algorithm>
#include <cassert>
#include <iostream>
#include <set>
#include <unordered_map>

void run(std::istream&, std::ostream&);

int main() {
	run(std::cin, std::cout);
	return 0;
}

class GameState {
public:
	uint8_t empty_cell_position, cells[16], shortest_distance, astar_heuristic;
	GameState();
	bool operator==(const GameState&) const;
	bool operator!=(const GameState&) const;
	struct HashFunc {
		size_t operator()(const GameState&) const;
	};
	struct Comparator {
		bool operator()(const GameState&, const GameState&) const;
	};
	void calc_heuristic();
	bool can_move_up() const;
	bool can_move_down() const;
	bool can_move_left() const;
	bool can_move_right() const;
	GameState move_up() const;
	GameState move_down() const;
	GameState move_left() const;
	GameState move_right() const;
	bool check() const;
};

bool find5thSolution(GameState&, std::string&);

void run(std::istream &in, std::ostream &out) {
	GameState start_game_state;
	unsigned short number;
	for (size_t index = 0; index < 16; index++) {
		in >> number;
		start_game_state.cells[index] = number;
		if (number == 0) {
			start_game_state.empty_cell_position = index;
		}
	}
	std::string result;
	if (find5thSolution(start_game_state, result)) {
		out << result.length() << std::endl << result;
	} else {
		out << -1;
	}
}

void proceed_astar(std::unordered_map<GameState, std::pair<GameState, char>, GameState::HashFunc>&,
std::set<GameState, GameState::Comparator>&, GameState&, GameState, char);

bool find5thSolution(GameState &start_state, std::string &result) {
	start_state.calc_heuristic();
	std::unordered_map<GameState, std::pair<GameState, char>, GameState::HashFunc> visited; //pair(parent, step)
	visited[start_state] = std::make_pair(start_state, 0);
	std::set<GameState, GameState::Comparator> pq;
	pq.insert(start_state);
	do {
		GameState current_state = *pq.begin();
		pq.erase(pq.begin());
		if (current_state.check()) {
			while (current_state != start_state) {
				auto pair = visited[current_state];
				result += pair.second;
				current_state = pair.first;
			}
			std::reverse(result.begin(), result.end());
			return true;
		}
		if (current_state.can_move_up()) {
			proceed_astar(visited, pq, current_state, current_state.move_up(), 'D');
		}
		if (current_state.can_move_down()) {
			proceed_astar(visited, pq, current_state, current_state.move_down(), 'U');
		}
		if (current_state.can_move_left()) {
			proceed_astar(visited, pq, current_state, current_state.move_left(), 'R');
		}
		if (current_state.can_move_right()) {
			proceed_astar(visited, pq, current_state, current_state.move_right(), 'L');
		}
	} while (!pq.empty());
	return false;
}

void proceed_astar(std::unordered_map<GameState, std::pair<GameState, char>, GameState::HashFunc> &visited,
std::set<GameState, GameState::Comparator> &pq, GameState &current_state, GameState next_state, char step) {
	auto it = visited.find(next_state);
	if (it == visited.end()) {
		visited[next_state] = std::make_pair(current_state, step);
		next_state.shortest_distance = current_state.shortest_distance + 1;
		pq.insert(next_state);
	} else if (next_state.shortest_distance > current_state.shortest_distance + 1) {
		pq.erase(it->first);
		next_state.shortest_distance = current_state.shortest_distance + 1;
		pq.insert(next_state);
	}
}

GameState::GameState(): empty_cell_position(0), cells{}, shortest_distance(0), astar_heuristic(0) {
}

bool GameState::operator==(const GameState &game_state) const {
	if (empty_cell_position != game_state.empty_cell_position) {
		return false;
	}
	for (size_t index = 0; index < 16; index++) {
		if (cells[index] != game_state.cells[index]) {
			return false;
		}
	}
	return true;
}

bool GameState::operator!=(const GameState &game_state) const {
	return !(*this == game_state);
}

bool GameState::can_move_up() const {
	return empty_cell_position > 3;
}

bool GameState::can_move_down() const {
	return empty_cell_position < 12;
}

bool GameState::can_move_left() const {
	return empty_cell_position % 4 != 0;
}

bool GameState::can_move_right() const {
	return empty_cell_position % 4 != 3;
}

GameState GameState::move_up() const {
	assert(can_move_up());
	GameState new_game_state(*this);
	new_game_state.empty_cell_position -= 4;
	std::swap(new_game_state.cells[empty_cell_position], new_game_state.cells[new_game_state.empty_cell_position]);
	new_game_state.calc_heuristic();
	return new_game_state;
}

GameState GameState::move_down() const {
	assert(can_move_down());
	GameState new_game_state(*this);
	new_game_state.empty_cell_position += 4;
	std::swap(new_game_state.cells[empty_cell_position], new_game_state.cells[new_game_state.empty_cell_position]);
	new_game_state.calc_heuristic();
	return new_game_state;
}

GameState GameState::move_left() const {
	assert(can_move_left());
	GameState new_game_state(*this);
	new_game_state.empty_cell_position--;
	std::swap(new_game_state.cells[empty_cell_position], new_game_state.cells[new_game_state.empty_cell_position]);
	new_game_state.calc_heuristic();
	return new_game_state;
}

GameState GameState::move_right() const {
	assert(can_move_right());
	GameState new_game_state(*this);
	new_game_state.empty_cell_position++;
	std::swap(new_game_state.cells[empty_cell_position], new_game_state.cells[new_game_state.empty_cell_position]);
	new_game_state.calc_heuristic();
	return new_game_state;
}

bool GameState::check() const {
	if (empty_cell_position != 15) {
		return false;
	}
	for (size_t index = 0; index < 15; index++) {
		if (cells[index] != index + 1) {
			return false;
		}
	}
	return true;
}

void GameState::calc_heuristic() {
	astar_heuristic = 0;
	for (size_t index = 0; index < 16; index++) {
		if (cells[index] != 0) {
			if (cells[index] < index + 1) {
				astar_heuristic += index + 1 - cells[index];
			} else {
				astar_heuristic += cells[index] - index - 1;
			}
		}
	}
}

size_t GameState::HashFunc::operator()(const GameState &game_state) const {
	size_t index = 15, power16 = 1, result = 0;
	do {
		result += game_state.cells[index] * power16;
		power16 *= 16;
	} while (index-- > 0);
	return result;
}

bool GameState::Comparator::operator()(const GameState &game_state1, const GameState &game_state2) const {
	if (game_state1.astar_heuristic == game_state2.astar_heuristic) {
		return game_state1.shortest_distance < game_state2.shortest_distance;
	} else {
		return game_state1.astar_heuristic < game_state2.astar_heuristic;
	}
}

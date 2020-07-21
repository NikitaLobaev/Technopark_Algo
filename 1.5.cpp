#include <iostream>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) > (b) ? (b) : (a))

void run(std::istream&, std::ostream&);

int main() {
	run(std::cin, std::cout);
	return 0;
}

struct Date {
	unsigned short day: 5, month: 4, year: 15;
	bool period_start; //начало периода общения или конец
};

size_t get_max_group(const Date*, size_t);

template <class Element, class Comparator>
void merge_sort(Element*, size_t, Comparator);

bool compare_dates(const Date&, const Date&);

void run(std::istream &in, std::ostream &out) {
	size_t count, index = 0;
	in >> count;
	count *= 2;
	auto dates = new Date[count];
	bool period_start = false;
	for (unsigned short day, month, year; index < count; index++) {
		in >> day >> month >> year;
		period_start = !period_start;
		dates[index].day = day;
		dates[index].month = month;
		if (period_start) {
			dates[index].year = year + 18;
		} else {
			Date max_death = dates[index - 1]; //нужна именно копия, не ссылка,
			max_death.year += 62;              //так как изменяем дату для дальнейшего сравнения
			dates[index].year = year;
			if (compare_dates(max_death, dates[index])) {
				dates[index] = max_death;
			} else if (!compare_dates(dates[index - 1], dates[index])) {
				count -= 2; //исключаем тех людей, которые вообще никогда не могли пересекаться
				index -= 2;
				continue;
			}
		}
		dates[index].period_start = period_start;
	}
	merge_sort(dates, count, compare_dates);
	out << get_max_group(dates, count);
	delete[] dates;
}

size_t get_max_group(const Date *date, size_t length) {
	size_t current_groups_count = 0, max_group_count = 0, index;
	for (index = 0; index < length; index++) {
		if (date[index].period_start) {
			current_groups_count++;
			max_group_count = max(max_group_count, current_groups_count);
		} else {
			current_groups_count--;
		}
	}
	return max_group_count;
}

bool compare_dates(const Date &date1, const Date &date2) {
	if (date1.year == date2.year) {
		if (date1.month == date2.month) {
			if (date1.day == date2.day) {
				return !date1.period_start;
			} else {
				return date1.day < date2.day;
			}
		} else {
			return date1.month < date2.month;
		}
	} else {
		return date1.year < date2.year;
	}
}

template <class Element, class Comparator>
void merge_sort(Element *array, size_t length, Comparator compare) {
	auto first_length = length / 2, second_length = length - first_length;
	auto left = array, right = array + first_length;
	if (first_length > 1) {
		merge_sort(array, first_length, compare);
	}
	if (second_length > 1) {
		merge_sort(right, second_length, compare);
	}
	auto tmp_array = new Element[length], current_position = tmp_array;
	Element *const left_end = right, *const right_end = array + length;
	while (left < left_end and right < right_end) {
		if (compare(*left, *right)) {
			*current_position++ = *left++;
		} else {
			*current_position++ = *right++;
		}
	}
	while (left < left_end) {
		*current_position++ = *left++;
	}
	while (right < right_end) {
		*current_position++ = *right++;
	}
	for (current_position = tmp_array; array < right_end; array++, current_position++) {
		*array = *current_position;
	}
	delete[] tmp_array;
}

#include <algorithm>
#include <execution>
#include <iostream>
#include <list>
#include <random>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>
#include <thread>
#include <future>

#include "log_duration.h"

using namespace std;

template <typename Strings>
void PrintStrings(const Strings& strings) {
	for (string_view s : strings) {
		cout << s << " ";
	}
	cout << endl;
}

string GenerateWord(mt19937& generator, int max_length) {
	const int length = uniform_int_distribution(1, max_length)(generator);
	string word;
	word.reserve(length);
	for (int i = 0; i < length; ++i) {
		word.push_back(uniform_int_distribution('a', 'z')(generator));
	}
	return word;
}

template <template <typename> typename Container>
Container<string> GenerateDictionary(mt19937& generator, int word_count, int max_length) {
	vector<string> words;
	words.reserve(word_count);
	for (int i = 0; i < word_count; ++i) {
		words.push_back(GenerateWord(generator, max_length));
	}
	return Container(words.begin(), words.end());
}

struct Reverser {
	void operator()(string& value) const {
		reverse(value.begin(), value.end());
	}
};

template <typename Container, typename Function>
void Test(string_view mark, Container keys, Function function) {
	LOG_DURATION(mark);
	function(keys, Reverser{});
}

#define TEST(function) Test(#function, keys, function<remove_const_t<decltype(keys)>, Reverser>)

//обычная функция если не передана даже политика
template <typename ForwardRange, typename Function> void ForEach(ForwardRange& range, Function function) {
	return for_each(execution::par, range.begin(), range.end(), function); // если без политики то базовую  функцию 
}

// проверку какую функцию вызвать - если и параллельная политика и итератор не произвольного доступа(не random то вызывается моя функция , в остальных случаях обычная )
template <typename ForwardRange, typename Function, typename Policy>
void ForEach(const Policy& policy, ForwardRange& range, Function function) {
	if constexpr (!(is_same_v<decay_t<random_access_iterator_tag>, typename ForwardRange::iterator>) && is_same_v<decay_t<Policy>>, execution::parallel_policy > ) {

		ForEach(policy, range, function); //если итератор не рандомный и политика параллельная то вызываю свою

	}
	else { //во всех остальных обычную   рандомный то обычную} 
		int size_ = range.size();
		unsigned int task_count = std::thread::hardware_concurrency();
		int chunc_size = size_ / task_count;
		int left_size = size_ % task_count;
		int first_chunc_size = 0;
		vector<future<void>> asyncs;
		if (left_size) {
			first_chunc_size = chunc_size + left_size;
		}
		auto left_boarder = range.begin();
		bool first = true;
		for (auto itr = left_boarder; itr != range.end(); next(itr, first_chunc_size)) {
			if (first) {
				auto begin_it = itr; // получают итератор на начало отрезка 
				std::advance(itr, first_chunc_size); // получаю итератор на конец  отрезка 
				asyncs.push_back(async([function, begin_it, itr] { for_each(begin_it, itr, function); }));
				first = false;
				left_boarder = itr;
				first_chunc_size = chunc_size;
			}
			else {
				auto begin_it = itr; // получают итератор на начало отрезка 
				std::advance(itr, first_chunc_size); // получаю итератор на конец  отрезка 
				asyncs.push_back(async([function, begin_it, itr] { for_each(begin_it, itr, function); }));
				left_boarder = itr;
			}
		}
		for (auto& task : asyncs) { task.get(); }


	}
}


	int main() {
		auto reverser = [](string& s) { reverse(s.begin(), s.end()); };

		list<string> strings_list = { "cat", "dog", "code" };

		ForEach(strings_list, reverser);
		PrintStrings(strings_list);
		// tac god edoc

		ForEach(execution::seq, strings_list, reverser);
		PrintStrings(strings_list);
		// cat dog code

		// единственный из вызовов, где должна работать ваша версия
		// из предыдущего задания
		ForEach(execution::par, strings_list, reverser);
		PrintStrings(strings_list);
		// tac god edoc

		vector<string> strings_vector = { "cat", "dog", "code" };

		ForEach(strings_vector, reverser);
		PrintStrings(strings_vector);
		// tac god edoc

		ForEach(execution::seq, strings_vector, reverser);
		PrintStrings(strings_vector);
		// cat dog code

		ForEach(execution::par, strings_vector, reverser);
		PrintStrings(strings_vector);
		// tac god edoc

		return 0;
	}

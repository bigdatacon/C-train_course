#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <string_view>
#include <vector>
#include <mutex>
#include <future>
#include <execution>




#include "log_duration.h"

using namespace std;

/*
Как вы, возможно, помните, алгоритм copy_if фильтрует элементы из данного диапазона и сохраняет подходящие в указанный выходной итератор, сохраняя их исходный порядок. Ваша задача — ускорить copy_if за счёт параллельности и отказа от требования сохранения порядка элементов.
Напишите шаблонную функцию CopyIfUnordered, которая:
принимает контейнер элементов и предикат — функцию, принимающую элемент и возвращающую bool;
возвращает вектор элементов, для которых предикат вернул true;
порядок элементов в итоговом векторе не имеет значения, но каждый элемент, удовлетворяющий предикату, должен присутствовать в результате ровно столько же раз, сколько в исходном контейнере.
В заготовке кода вам дана последовательная версия этой функции. Ускорьте её.
*/

string GenerateWord(mt19937& generator, int max_length) {
	const int length = uniform_int_distribution(1, max_length)(generator);
	string word;
	word.reserve(length);
	for (int i = 0; i < length; ++i) {
		//word.push_back(uniform_int_distribution('a', 'z')(generator));
		word.push_back(uniform_int_distribution(0, 26)(generator) + 'a');

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

template <typename Strings, typename Predicate, typename Function>
void Test(string_view mark, const Strings& strings, Predicate predicate, Function function) {
	LOG_DURATION(mark);
	const auto result = function(strings, predicate);
	cout << result.size() << " " << result[5].substr(0, 5) << endl;
}

#define TEST(function) \
    Test(#function, strings, predicate, function<vector<string>, decltype(predicate)>)

/*template <typename Container, typename Predicate>
vector<typename Container::value_type> CopyIfUnordered(const Container& container,
													   Predicate predicate) {
	vector<typename Container::value_type> result;
	for (const auto& value : container) {
		if (predicate(value)) {
			result.push_back(value);
		}
	}
	return result;
}*/


/*
vector<int> CountFrequenciesForEachSeparateMutexes(const vector<int>& numbers) {
	vector<int> freqs(MAX_VALUE + 1);
	vector<mutex> freqs_mutexes(MAX_VALUE + 1);
	for_each(
		execution::par,
		numbers.begin(), numbers.end(),
		[&freqs, &freqs_mutexes](int number) {
			lock_guard guard(freqs_mutexes[number]);
			++freqs[number];
		}
	);
	return freqs;
}
*/

/*template <typename Container, typename Predicate>
vector<typename Container::value_type> CopyIfUnordered(const Container& container,
													   Predicate predicate) {
	vector<mutex> freqs_mutexes(container.size() + 1);
	vector<typename Container::value_type> result;

	int index_mutexex = 0;
	for_each(
		execution::par,
		container.begin(), container.end(),
		[&result, &freqs_mutexes, &predicate, &index_mutexex](auto number) {
			lock_guard guard(freqs_mutexes[index_mutexex ]);
			if (predicate(number)){
			result.push_back(number);}
			++ index_mutexex;
		}
	);


	return result;
}*/

/*template <typename Container, typename Predicate>
vector<typename Container::value_type> CopyIfUnordered(const Container& container,
													   Predicate predicate) {
	mutex mutex;
	vector<typename Container::value_type> result;
	for_each(
		execution::par,
		container.begin(), container.end(),
		[&result, &predicate, &mutex](auto number) {
			if (predicate(number)){
				lock_guard guard(mutex);
				result.push_back(number);
			}
		}
	);


	return result;
}*/

template <typename Container, typename Predicate>
vector<typename Container::value_type> CopyIfUnordered(const Container& container,
	Predicate predicate) {

	vector<typename Container::value_type> result;
	result.reserve(container.size());
	mutex m_;

	//int index_mutexex = 0;
	for_each(
		execution::par,
		container.begin(), container.end(),
		[&result, &predicate, &m_](const auto& number) {

			if (predicate(number)) {
				typename Container::value_type* insert_;
				{
					lock_guard guard(m_);
					insert_ = &result.emplace_back();
				}
				*insert_ = number;

			}
		}
	);
    return result;
}





int main() {
	vector<int> numbers(1'000);
	iota(numbers.begin(), numbers.end(), 0);

	const vector<int> even_numbers = CopyIfUnordered(numbers, [](int number) {
		return number % 2 == 0;
		});
	for (const int number : even_numbers) {
		cout << number << " "s;
	}
	cout << endl;
	// выведет все чётные числа от 0 до 999

	mt19937 generator;

	const auto strings = GenerateDictionary<vector>(generator, 50'000, 3000);
	auto predicate = [](const string& s) {
		return count(s.begin(), s.end(), 'a') < 100;
	};

	TEST(CopyIfUnordered);
}

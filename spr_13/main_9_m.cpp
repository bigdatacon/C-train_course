#include <algorithm>
#include <cassert>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>
 
#include <numeric>
#include <cmath>
using namespace std;
 
 
 
 
 
template <typename InputIt, typename OutSumType, typename OutSqSumType, typename OutMaxType>
void ComputeStatistics(InputIt first, InputIt last, OutSumType& out_sum, OutSqSumType& out_sq_sum, OutMaxType& out_max) {
	using Elem = std::decay_t<decltype(*first)>;
 
	constexpr bool need_sum = !is_same_v<typename std::remove_reference_t<OutSumType>, const nullopt_t>;
	constexpr bool need_sq_sum = !is_same_v<typename std::remove_reference_t<OutSqSumType>, const nullopt_t>;
	constexpr bool need_max = !is_same_v<typename std::remove_reference_t<OutMaxType>, const nullopt_t>;
 
 
    if constexpr (need_max) {
		//Elem out_max{};
        for (; first != last; ++first) {
        const Elem& value = *first;
            if (!out_max || value > out_max) {
				out_max = value;
			}
 
        }
 
	}
 
	if constexpr (need_sum) {
		//Elem out_sum{};
        const Elem& value = *first;
        for (; first != last; ++first) {
            if (out_sum) {
                out_sum += value;
            }
            else {
                out_sum = value;
            }
        }
	}
 
	if constexpr (need_sq_sum) {
        const Elem& value = *first;
		//Elem out_sq_sum{};
        for (; first != last; ++first){
            if (out_sq_sum ) {
				out_sq_sum += value*value;
			}
            else {
			out_sq_sum += value * value;
            }
 
        }
	}
 
 
}
 
 
struct OnlySum {
	int value;
};
 
OnlySum operator+(OnlySum l, OnlySum r) {
	return { l.value + r.value };
}
OnlySum& operator+=(OnlySum& l, OnlySum r) {
	return l = l + r;
}
 
int main() {
	vector input = { 1, 2, 3, 4, 5, 6 };
	int sq_sum;
	std::optional<int> max;
 
	// Переданы выходные параметры разных типов - std::nullopt_t, int и std::optional<int>
	ComputeStatistics(input.begin(), input.end(), nullopt, sq_sum, max);
 
	assert(sq_sum == 91 && max && *max == 6);
 
	vector<OnlySum> only_sum_vector = { {100}, {-100}, {20} };
	OnlySum sum;
 
	// Поданы значения поддерживающие только суммирование, но запрошена только сумма
	ComputeStatistics(only_sum_vector.begin(), only_sum_vector.end(), sum, nullopt, nullopt);
 
	assert(sum.value == 20);
}

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

    /*constexpr bool need_sum = !is_same_v<typename std::remove_reference_t<OutSumType>, nullopt_t>;
    constexpr bool need_sq_sum = !is_same_v<typename std::remove_reference_t<OutSqSumType>, nullopt_t>;
    constexpr bool need_max = !is_same_v<typename std::remove_reference_t<OutMaxType>, nullopt_t>;*/

    constexpr bool need_sum = !std::is_same_v<typename std::remove_reference_t<OutSumType>, std::optional<Elem>>;
    constexpr bool need_sq_sum = !std::is_same_v<typename std::remove_reference_t<OutSqSumType>, std::optional<Elem>>;
    constexpr bool need_max = !std::is_same_v<typename std::remove_reference_t<OutMaxType>, std::optional<Elem>>;


    std::optional<Elem> sum_tmp = nullopt;
    std::optional<Elem> sq_sum_tmp = nullopt;
    std::optional<Elem> max_tmp = nullopt;

    if constexpr (need_max) {
        for (; first != last; ++first) {
            const Elem& value = *first;
            if (/*!max_tmp*/ std::is_same_v<typename std::remove_reference_t<decltype(max_tmp)>, std::nullopt_t> || value > *max_tmp) {
                max_tmp = value;
            }
        }
    }

    if constexpr (need_sum) {
        
        for (; first != last; ++first) {
            const Elem& value = *first;
            if (/*sum_tmp*/ !std::is_same_v<typename std::remove_reference_t<decltype(sum_tmp)>, std::nullopt_t>) {
                *sum_tmp += value;
            }
            else {
                sum_tmp = value;
            }
        }
    }

    if constexpr (need_sq_sum) {
        
        for (; first != last; ++first) {
            const Elem& value = *first;
            if (/*sq_sum_tmp*/ !std::is_same_v<typename std::remove_reference_t<decltype(sq_sum_tmp)>, std::nullopt_t>) {
                *sq_sum_tmp += value * value;
            }
            else {
                sq_sum_tmp = value * value;
            }
        }
    }

    /*
    if constexpr (need_sum && sum_tmp) {
        out_sum = std::move(*sum_tmp);
    }
    if constexpr (need_sq_sum && sq_sum_tmp) {
        out_sq_sum = std::move(*sq_sum_tmp);
    }
    if constexpr (need_max && max_tmp) {
        out_max = std::move(*max_tmp);
    }
    */
    
}



struct OnlySum {
    int value;
    bool operator>(const OnlySum& other) const {
        return value > other.value;
    }

    OnlySum& operator=(const OnlySum& other) {
        value = other.value;
        return *this;
    }
};

OnlySum operator+(OnlySum l, OnlySum r) {
    return { l.value + r.value };
}
OnlySum& operator+=(OnlySum& l, OnlySum r) {
    return l = l + r;
}

OnlySum operator*(OnlySum l, OnlySum r) {
    return { l.value * r.value };
}

OnlySum& operator*=(OnlySum& l, OnlySum r) {
    return l = l * r;
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

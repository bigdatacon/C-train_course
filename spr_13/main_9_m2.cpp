#include <algorithm>
#include <cassert>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>
#include <iostream>

#include <numeric>
#include <cmath>
using namespace std;




template <typename InputIt, typename OutSum, typename OutSqSum, typename OutMax>
void ComputeStatistics(InputIt first, InputIt last, OutSum& out_sum, OutSqSum& out_sq_sum,
    OutMax& out_max) {
    using Elem = std::decay_t<decltype(*first)>;

    constexpr bool need_sum = !is_same_v<OutSum, const nullopt_t>;
    constexpr bool need_sq_sum = !is_same_v<OutSqSum, const nullopt_t>;
    constexpr bool need_max = !is_same_v<OutMax, const nullopt_t>;


    /*constexpr bool need_sum = !is_same_v<typename std::remove_reference_t<OutSumType>, nullopt_t>;
    constexpr bool need_sq_sum = !is_same_v<typename std::remove_reference_t<OutSqSumType>, nullopt_t>;
    constexpr bool need_max = !is_same_v<typename std::remove_reference_t<OutMaxType>, nullopt_t>;*/
    /*
    constexpr bool need_sum = !std::is_same_v<typename std::remove_reference_t<OutSumType>, std::optional<Elem>>;
    constexpr bool need_sq_sum = !std::is_same_v<typename std::remove_reference_t<OutSqSumType>, std::optional<Elem>>;
    constexpr bool need_max = !std::is_same_v<typename std::remove_reference_t<OutMaxType>, std::optional<Elem>>;
    */
    /*
    constexpr bool need_sum = std::is_same_v<typename std::remove_reference_t<OutSumType>, std::optional<Elem>>;
    constexpr bool need_sq_sum = std::is_same_v<typename std::remove_reference_t<OutSqSumType>, std::optional<Elem>>;
    constexpr bool need_max = std::is_same_v<typename std::remove_reference_t<OutMaxType>, std::optional<Elem>>;
    */

    /*
    constexpr bool need_sum = std::is_same_v<typename std::remove_reference_t<OutSumType>, std::optional<Elem> > || std::is_same_v<typename std::remove_reference_t<OutSumType>, Elem>;
    constexpr bool need_sq_sum = std::is_same_v<typename std::remove_reference_t<OutSqSumType>, std::optional<Elem> > || std::is_same_v<typename std::remove_reference_t<OutSqSumType>, Elem>;
    constexpr bool need_max = std::is_same_v<typename std::remove_reference_t<OutMaxType>, std::optional<Elem> > || std::is_same_v<typename std::remove_reference_t<OutMaxType>, Elem>;
    */


    std::optional<Elem> sum_tmp = nullopt;
    std::optional<Elem> sq_sum_tmp = nullopt;
    std::optional<Elem> max_tmp = nullopt;


    for (; first != last; ++first) {
        const Elem& value = *first;
        if (sum_tmp) {
            *sum_tmp += *first;
        }
        else {
            sum_tmp = value;
        }


        if (max_tmp || value > *max_tmp) {
                max_tmp = value;
            }

        if (sq_sum_tmp) {
            *sq_sum_tmp += value * value;
        }
        else {
            sq_sum_tmp = value * value;
        }
    }

    

    if constexpr (need_sum) {
        out_sum = std::move(*sum_tmp);
    }
    if constexpr (need_sq_sum) {
        out_sq_sum = std::move(*sq_sum_tmp);
    }
    if constexpr (need_max) {
        out_max = std::move(*max_tmp);
    }


}



struct OnlySum {
    int value;

    OnlySum() = default;  // Добавьте явное объявление конструктора по умолчанию

    OnlySum(const OnlySum& other) = default;  // Явное объявление конструктора копирования

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

    // РџРµСЂРµРґР°РЅС‹ РІС‹С…РѕРґРЅС‹Рµ РїР°СЂР°РјРµС‚СЂС‹ СЂР°Р·РЅС‹С… С‚РёРїРѕРІ - std::nullopt_t, int Рё std::optional<int>
    ComputeStatistics(input.begin(), input.end(), nullopt, sq_sum, max);

    assert(sq_sum == 91 && max && *max == 6);

    vector<OnlySum> only_sum_vector = { {100}, {-100}, {20} };
    OnlySum sum;

    // РџРѕРґР°РЅС‹ Р·РЅР°С‡РµРЅРёСЏ РїРѕРґРґРµСЂР¶РёРІР°СЋС‰РёРµ С‚РѕР»СЊРєРѕ СЃСѓРјРјРёСЂРѕРІР°РЅРёРµ, РЅРѕ Р·Р°РїСЂРѕС€РµРЅР° С‚РѕР»СЊРєРѕ СЃСѓРјРјР°
    ComputeStatistics(only_sum_vector.begin(), only_sum_vector.end(), sum, nullopt, nullopt);

    assert(sum.value == 20);

    std::cout << "Success" << std::endl;

    return 0;
}

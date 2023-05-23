#include <algorithm>
#include <cassert>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

using namespace std;

// напишите функцию ComputeStatistics, принимающую 5 параметров:
// два итератора, выходное значение для суммы, суммы квадратов и максимального элемента

struct OnlySum {
    int value;
};

OnlySum operator+(OnlySum l, OnlySum r) {
    return {l.value + r.value};
}
OnlySum& operator+=(OnlySum& l, OnlySum r) {
    return l = l + r;
}

int main() {
    vector input = {1, 2, 3, 4, 5, 6};
    int sq_sum;
    std::optional<int> max;

    // Переданы выходные параметры разных типов - std::nullopt_t, int и std::optional<int>
    ComputeStatistics(input.begin(), input.end(), nullopt, sq_sum, max);

    assert(sq_sum == 91 && max && *max == 6);

    vector<OnlySum> only_sum_vector = {{100}, {-100}, {20}};
    OnlySum sum;

    // Поданы значения поддерживающие только суммирование, но запрошена только сумма
    ComputeStatistics(only_sum_vector.begin(), only_sum_vector.end(), sum, nullopt, nullopt);

    assert(sum.value == 20);
}

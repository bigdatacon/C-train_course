#include <iostream>
#include <string>
#include <algorithm>
#include <set>
#include <cstdlib>
#include <cmath>

using namespace std;



set<int>::const_iterator /*void*/ FindNearestElement2(const set<int>& numbers, int border) {
    if (numbers.empty()) {
        // Сначала проверяем, что множество пустое
        return numbers.end();
    }
    // Ищем наименьший элемент больший, либо равный border
    auto it_min = lower_bound(numbers.begin(), numbers.end(), border);
    // Ищем наибольший элемент меньший, либо равный border
    // Для этого обращаем предикат сравнения и идем по множеству в обратном порядке
    // (begin() ... end() перебирают его от меньшего к большему а
    // rbegin() ... rend() от большего к меньшему).
    auto it_max = lower_bound(numbers.rbegin(), numbers.rend(), border, [](int a, int b){return a > b;});
    if (it_min == numbers.end()) {
        // Не найдено элементов больше border - возвращаем ближайший меньше
        return numbers.find(*it_max);
    }
    if (it_max == numbers.rend()) {
        // Не найдено элементов меньше border - возвращаем ближайший больше
        return it_min;
    }
    // Сравниваем расстояния от найденных элементов до border и возвращаем тот что ближе
    if (border - * it_max <= * it_min - border) {
        return numbers.find(*it_max);
    } else {
        return it_min;
    }
}

int main() {
    set<int> numbers = {1, 4, 6};
    cout << *FindNearestElement(numbers, 0) << " " << *FindNearestElement(numbers, 3) << " "
         << *FindNearestElement(numbers, 5) << " " << *FindNearestElement(numbers, 6) << " "
         << *FindNearestElement(numbers, 100) << endl;
    set<int> empty_set;
    cout << (FindNearestElement(empty_set, 8) == end(empty_set)) << endl;
    return 0;
}


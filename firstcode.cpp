#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;
/*
Задание 3
Обобщим предыдущую задачу. Теперь префикс будет не одним символом типа char, а строкой с любым количеством символов.


Эта задача аналогична предыдущей, но с одной тонкостью. Как составить ближайшую строку, которая будет больше prefix? Просто увеличить на единицу последний символ.

Пример вывода
moscow motovilikha
2 2
3 3 
*/


template <typename RandomIt>
pair<RandomIt, RandomIt> FindStartsWith(RandomIt range_begin, RandomIt range_end, string prefix) {
    auto left = lower_bound(range_begin, range_end, prefix); // левый получинтерва
    char next_prefix = static_cast<char>(prefix.back() + 1); // нахожу букву следуюшую за последней
    string new_string = prefix.substr(0,-1)+next_prefix; // составляю новую строку в которой все неизменно кроме //последнй буквы
    auto right = lower_bound(range_begin, range_end,  new_string);

    return {left, right};
}



int main() {
    const vector<string> sorted_strings = {"moscow", "motovilikha", "murmansk"};
    const auto mo_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), "mo");
    for (auto it = mo_result.first; it != mo_result.second; ++it) {
        cout << *it << " ";
    }
    cout << endl;
    const auto mt_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), "mt");
    cout << (mt_result.first - begin(sorted_strings)) << " " << (mt_result.second - begin(sorted_strings)) << endl;
    const auto na_result = FindStartsWith(begin(sorted_strings), end(sorted_strings), "na");
    cout << (na_result.first - begin(sorted_strings)) << " " << (na_result.second - begin(sorted_strings)) << endl;
    return 0;
} 

#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

using namespace std;
/*
Задание 1
Напишите функцию GetPermutations, которая получает в качестве аргумента итератор на начало контейнера и итератор на его конец и возвращает вектор строк со всеми возможными перестановками элементов этого контейнера. Элементы перестановок разделяйте пробелами.
Мы немного обновили для вашего удобства функцию PrintRange. Теперь это функция  PrintRangeToString, и она печатает не на экран, а в строку. Используйте в решении эту обновлённую версию.
Порядок, в котором функция возвратит перестановки не важен.

Пример вывода
3 2 1
3 1 2
2 3 1
2 1 3
1 3 2
1 2 3 


ПОДСКАЗКА 
Разберитесь с этими строками-ребусами и найдёте алгоритмы, которые помогут вам решить задачу.
o r s t
r e e a t r g
n o i t a t u m r e p _ v e r p
n o i t a t u m r e p _ v e p r
n o i t a t u m r e p _ r v p e
n o i t a t u m r e p _ r v e p
n o i t a t u m r e p _ r p v e
n o i t a t u m r e p _ r p e v
n o i t a t u m r e p _ r e v p
n o i t a t u m r e p _ r e p v
n o i t a t u m r e p _ p v r e 
*/


// функция, записывающая элементы диапазона в строку
template <typename It>
string PrintRangeToString(It range_begin, It range_end) {
    // удобный тип ostringstream -> https://ru.cppreference.com/w/cpp/io/basic_ostringstream
    ostringstream out;
    for (auto it = range_begin; it != range_end; ++it) {
        out << *it << " "s;
    }
    out << endl;
    // получаем доступ к строке с помощью метода str для ostringstream
    return out.str();
}
 
// Мое решение 
template <typename It>
vector<string> GetPermutations(It range_begin, It range_end){

    
}



int main() {
    vector<int> permutation(3);
    // iota             -> http://ru.cppreference.com/w/cpp/algorithm/iota
    // Заполняет диапазон последовательно возрастающими значениями
    iota(permutation.begin(), permutation.end(), 1);
    auto result = GetPermutations(permutation.begin(), permutation.end());
    for (const auto& s : result) {
        cout << s;
    }
    return 0;
}

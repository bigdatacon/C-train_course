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
    vector<string> result;
    string s=PrintRangeToString(range_begin,  range_end);
    std::sort(s.begin(), s.end());
    do {
        //std::cout << s << '\n';
        result.push_back(s);
    } while(std::next_permutation(s.begin(), s.end()));
    return result;
    
}


// не мое 
/*
string w(string s){
    string q="";
    bool e=true;
    for(char t:s){
        if(!e){
            q+=" ";
            q+=t;
        }else{
            q+=t;
            e=false;
        }
    }
    return q;
}

template<typename It>
vector<string> GetPermutations(It range_begin, It range_end){
    vector<string> qwe;
    string s=PrintRangeToString(range_begin,  range_end);
    sort(s.begin(), s.end(), greater<char>());
    do {
        qwe.push_back(s);
    } while(prev_permutation(s.begin(), s.end()));
    for(string& s:qwe){
        s=w(s);
    }
    return qwe;
} */


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

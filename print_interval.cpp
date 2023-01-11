#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <set>
using namespace std;
/*
Заготовки кода здесь нет — вы легко справитесь без неё. А вот решение не забудьте сохранить — оно
вам понадобится в следующем задании.
*/

/*
Задание 1
Напишите функцию PrintRange, которая была бы шаблоном от типа итератора и принимала полуинтервал итераторов, а затем выводила бы значение элемента. Можно предполагать, что оператор вывода для элемента контейнера определён. Пусть элементы выводятся через пробел. В конце вывода должен быть символ конца строки.
Функция PrintRange полезная. Сохраните решение у себя локально или в своём git-хранилище. Оно вам ещё много раз пригодится.
Пример вызова функции
int main() {
    cout << "Test1"s << endl;
    set<int> test1 = {1, 1, 1, 2, 3, 4, 5, 5};
    PrintRange(test1.begin(), test1.end());
    cout << "Test2"s << endl;
    vector<int> test2 = {}; // пустой контейнер
    PrintRange(test2.begin(), test2.end());
    cout << "End of tests"s << endl;
} 
Пример вывода
Test1
1 2 3 4 5
Test2
End of tests 

Подсказка 
Помните, что итератор — это набор определённых действий, и операторов сравнения среди них нет. Но есть !=.
А вот как должно выглядеть начало вашей функции:
template <typename It>
void PrintRange(It range_begin, It range_end) 
*/
template <typename It>
void PrintRange(It range_begin, It range_end){
    for (auto it = range_begin; it != range_end; it++) {
            cout << *it << endl;
        }
    }


int main() {
    cout << "Test1"s << endl;
    set<int> test1 = {1, 1, 1, 2, 3, 4, 5, 5};
    PrintRange(test1.begin(), test1.end());
    cout << "Test2"s << endl;
    vector<int> test2 = {}; // пустой контейнер
    PrintRange(test2.begin(), test2.end());
    cout << "End of tests"s << endl;
} 


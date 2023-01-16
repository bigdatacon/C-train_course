#include <iostream>
#include <string>
#include <algorithm>
#include <set>
#include <cstdlib>
#include <cmath>

using namespace std;
/*
Задание 1
Напишите функцию FindNearestElement для множества целых чисел numbers и данного числа border. Она должна возвращать итератор на элемент множества, ближайший к border. Если ближайших элементов несколько, верните итератор на наименьший из них. Если множество пустое, верните итератор на конец. Для поиска примените методы поиска по множеству. Они более эффективны, чем простой перебор элементов.

Ближайшим элементом может оказаться элемент меньше, чем border, сам border или элемент больше border. Используйте lower_bound, чтобы найти позицию элемента, который будет не меньше, чем border. Найденный элемент будет равным border или ближайшим с бó‎льшей стороны. Если возможно, сделайте шаг назад и найдите ближайший элемент с меньшей стороны. Останется только сравнить, насколько они отстают от border, и выбрать ближайший. Когда вы в начале контейнера, шагать назад не получится. 
Используйте метод контейнера lower_bound, а не общую функцию, чтобы ускорить свой код.
*/

// первоначальная функция 
/*set<int>::const_iteratorFindNearestElement(const set<int>& numbers, int border) {
    // set<int>::const_iterator — тип итераторов для константного множества целых чисел
}*/


set<int>::const_iterator /*void*/ FindNearestElement(const set<int>& numbers, int border) {
    // set<int>::const_iterator — тип итераторов для константного множества целых чисел
    // если множество не пустое 
    if (!numbers.empty()){
    set<int>::const_iterator some_set;
    auto it = lower_bound(numbers.begin(), numbers.end(), border);
    cout << "Номер позиции в векторе "s << distance(numbers.begin(), it) << endl;
    cout << "Значение элемента "s << *it << endl; 
        
    auto it_min  = lower_bound(numbers.begin(), it-1, it), border); // Если возможно, сделайте шаг назад и найдите ближайший элемент с меньшей стороны. 
    
        //Останется только сравнить, насколько они отстают от border, и выбрать ближайший
    if (abs(border- it))>(abs(border- it_min)){cout << "Значение меньшего элемента "s << *it_min << endl; }
        else {cout << "Значение меньшего элемента "s << *it << endl;}
        
    //some_set.insert(it);
    //some_set.insert(distance(numbers.begin(), it));
    //return *it;
    cout << "return some wrong : ";
    return numbers.end();  // тут нужно вернуть либо *it либо *it_min Но не работает
    }
    else  // если множество пустое возвращаю последний элемент 
    cout << "return some wrong : ";
    return numbers.end();
   
    
}


int main() {
    set<int> numbers = {7,3,1 ,4, 6};
    //set<int> numbers = {};
    //set<int> numbers = {1, 4, 6};
    //cout << *FindNearestElement(numbers, 4) << " ";
    cout << *FindNearestElement(numbers, 0) << " ";
    /*
    cout << *FindNearestElement(numbers, 0) << " " << *FindNearestElement(numbers, 3) << " "
         << *FindNearestElement(numbers, 5) << " " << *FindNearestElement(numbers, 6) << " "
         << *FindNearestElement(numbers, 100) << endl;
    set<int> empty_set;
    cout << (FindNearestElement(empty_set, 8) == end(empty_set)) << endl;
    */
    return 0;
}

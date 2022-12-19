#include <iostream>
#include <vector>
#include <set>
using namespace std;

/*
Научите функцию вывода работать не только с векторами, но и со множествами.

Вывод
Белка, Георгий, Мурка, Рюрик 
Шаблонизировать функцию типом контейнера вместо типа элемента не получится. Теперь даже при выводе простого числа компилятор не знает, вызывать ли ему стандартную функцию вывода числа или сделать вид, что число — это контейнер. Поэтому для начала ограничьтесь двумя версиями функции: для вектора и для множества.
*/

template <typename Term >
ostream& operator<<(ostream& out,  const set<Term>& container) {
    bool first = true;
    for ( const Term& element : container) {
        if (!first){
        out << ", "s;}
        
        first = false;
        
        out << element;
    }
    return out;
}  
   

int main() {
    /*const vector<int> ages = {10, 5, 2, 12};
    cout << ages << endl;*/
    const  set<string> cats = {"Мурка"s, "Белка"s, "Георгий"s, "Рюрик"s};
    cout << cats << endl;
    return 0;
}

#include <iostream>
#include <vector>
#include <set>
using namespace std;

/*
Сделайте вывод более нарядным: выводите содержимое вектора в квадратных скобках, а множества — в фигурных. Функция Print должна остаться прежней и не обрамлять вывод в скобки.

Не правьте функцию Print: достаточно добавить вывод скобок в каждую версию перегруженного operator<<. Не забудьте, что выводить нужно в out, а не в cout.
*/


template <typename Container>
void Print(ostream& out,  const Container& container) {
    bool first = true;
    for ( const auto& element : container) {
        if (!first){
        out << ", "s;}
        
        first = false;
        
        out <<  element;
    }
}

    
    
template <typename Term >
ostream& operator<<(ostream& out,  const set<Term>& container) {
    Print(out, container);
    return  out;
}  

template <typename Term >
ostream& operator<<(ostream& out,  const vector<Term>& container) {
    Print(out, container);
    //return static_cast<string>(out) + "}"s;
    return out;
}  
   

int main() {
    const vector<int> ages = {10, 5, 2, 12};
    cout << ages << endl;
    const  set<string> cats = {"Мурка"s, "Белка"s, "Георгий"s, "Рюрик"s};
    cout << cats << endl;
    return 0;
}

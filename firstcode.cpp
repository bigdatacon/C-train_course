#include <iostream>
#include <vector>
#include <set>
using namespace std;

/*
Научитесь выводить словари. Содержимое словаря должно быть заключено в угловые скобки, а каждая пара ключ-значение — в круглые.
Пусть функция Print выводит словарь без угловых скобок. Ни для каких других типов, кроме векторов, множеств и словарей, вызывать Print не нужно.
Напишите ещё одну версию operator<< с двумя шаблонными параметрами — для map<Key, Value>.
Функция Print, итерируясь по словарю, будет пытаться вывести пару — напишите и для неё operator<<.
При вызове одной функции из другой компилятор «видит» только те из них, что написаны выше. Поэтому выше всех должен быть оператор вывода пары, а сразу за ним — универсальная Print.
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
    out << "{"s;
    Print(out, container);
    out << "}"s;
    return  out;
}  

template <typename Term >
ostream& operator<<(ostream& out,  const vector<Term>& container) {
    out << "["s;
    Print(out, container);
    out << "]"s;
    return out;
}  
   

int main() {
    const vector<int> ages = {10, 5, 2, 12};
    cout << ages << endl;
    const  set<string> cats = {"Мурка"s, "Белка"s, "Георгий"s, "Рюрик"s};
    cout << cats << endl;
    return 0;
}

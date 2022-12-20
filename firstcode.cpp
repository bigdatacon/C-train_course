#include <iostream>
#include <vector>
#include <set>
#include <map>
using namespace std;

/*
Научитесь выводить словари. Содержимое словаря должно быть заключено в угловые скобки, а каждая пара ключ-значение — в круглые.
Пусть функция Print выводит словарь без угловых скобок. Ни для каких других типов, кроме векторов, множеств и словарей, вызывать Print не нужно.
{Белка, Георгий, Мурка, Рюрик}
[10, 5, 2, 12]
<(Белка, 5), (Георгий, 2), (Мурка, 10), (Рюрик, 12)> 
Напишите ещё одну версию operator<< с двумя шаблонными параметрами — для map<Key, Value>.
Функция Print, итерируясь по словарю, будет пытаться вывести пару — напишите и для неё operator<<.
При вызове одной функции из другой компилятор «видит» только те из них, что написаны выше.
Поэтому выше всех должен быть оператор вывода пары, а сразу за ним — универсальная Print.
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

// пишу версию оператора для словаря
/*template <typename Key,  typename Value>
ostream& operator<<(ostream& out,  const map<Key, Value>& container) {
    bool first = true;
    for ( const auto& [key, value] : container) {
        if (!first){
        out << ", "s;}
        
        first = false;      
        out << "("s;
        out <<  key ;
        out << " "s;
        out << value;
        out << ")"s;
        
    }
    return  out;
} */

template <typename Key,  typename Value>
void PrintMap(ostream& out,  const map<Key, Value>& container) {
    bool first = true;
    for ( const auto& [key, value] : container) {
        if (!first){
        out << ", "s;}
        
        first = false;      
        out << "("s;
        out <<  key ;
        out << ", "s;
        out << value;
        out << ")"s;
        
    }
} 
template <typename Key,  typename Value>
ostream& operator<<(ostream& out,  const map<Key, Value>& container) {
    out << "<"s;
    PrintMap(out, container);
    out << ">"s;
    return  out;
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
    /*const vector<int> ages = {10, 5, 2, 12};
    cout << ages << endl;
    const  set<string> cats = {"Мурка"s, "Белка"s, "Георгий"s, "Рюрик"s};
    cout << cats << endl;
    return 0;*/
    
    const set<string> cats = {"Мурка"s, "Белка"s, "Георгий"s, "Рюрик"s};
    cout << cats << endl;
    const vector<int> ages = {10, 5, 2, 12};
    cout << ages << endl;
    const map<string, int> cat_ages = {
                                        {"Мурка"s, 10}, 
                                        {"Белка"s, 5},
                                        {"Георгий"s, 2}, 
                                        {"Рюрик"s, 12}
                                    };
    cout << cat_ages<< endl;
    return 0;
}

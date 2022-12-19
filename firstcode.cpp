#include <iostream>
#include <vector>
#include <set>
using namespace std;

/*
Избавьтесь от дублирования кода. Вынесите общую функциональность вывода в шаблонную функцию Print, принимающую ссылку на поток вывода и объект шаблонного типа, элементы которого нужно вывести. Это означает, что последовательный вывод элементов контейнера теперь должен располагаться внутри функции Print. Вызовите её из операторов вывода вектора и множества.

Не забудьте назвать вспомогательную функцию Print, первым её аргументом сделать ссылку на поток, а вторым — контейнер шаблонного типа. Точно так же вы делали с набором документов Documents.
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

template <typename Term >
ostream& operator<<(ostream& out,  const vector<Term>& container) {
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
    const vector<int> ages = {10, 5, 2, 12};
    cout << ages << endl;
    const  set<string> cats = {"Мурка"s, "Белка"s, "Георгий"s, "Рюрик"s};
    cout << cats << endl;
    return 0;
}

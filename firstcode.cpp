#include <iostream>
#include <vector>
using namespace std;

/*
Вы могли заметить лишний пробел, который выводится после последнего элемента. Избавьтесь от него и замените пробел на запятую с пробелом. Код из предыдущей задачи теперь должен выводить 10, 5, 2, 12.

Выводите ", "s перед каждым элементом кроме первого. Понять, что элемент первый, поможет bool-переменная.
*/

template <typename Term >
ostream& operator<<(ostream& out, const set<Term>& container) {
    bool first = true;
    for (const Term& element : container) {
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
    const set<string> cats = {"Мурка"s, "Белка"s, "Георгий"s, "Рюрик"s};
    cout << cats << endl;
    return 0;
}

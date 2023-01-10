#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
/*
Задание 2
Как вы помните, итераторы даны нам для гибкости кода и для простоты внесения изменений. Пусть теперь вектор будет хранить не просто строки, а структуры. Найдите первый и единственный в данном случае язык, начинающийся на “J”. Попробуйте самостоятельно разобраться, как правильно вывести результат на экран. Всё так же продолжайте считать, что искомый элемент в контейнере есть.

Java, 24 
Чтобы получить доступ к отдельным полям структуры через итератор, используйте ->.
*/

using namespace std;

struct Lang {
    string name;
    int age;
};

int main() {
    vector<Lang> langs = {{"Python"s, 29}, {"Java"s, 24}, {"C#"s, 20}, {"Ruby"s, 25}, {"C++"s, 37}};
    // Выведите первый язык, начинающийся на J, используя алгоритм find_if
    auto is_tru = [](Lang i){ 
        auto f = i->name;
        //return i.name[0] == 'J';
        return f[0] == 'J';
    };

    //auto result3 = find_if(begin(langs), end(langs), is_tru);
    auto result3 = find_if(langs.begin(), langs.end(), is_tru);
    //cout <<*result3 << endl;
    cout <<*result3 << endl;
}

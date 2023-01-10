#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <algorithm>
#include <iterator>

using namespace std;

/*
Задание 1
Научимся применять новый алгоритм find_if. Результат его работы — итератор, указывающий на первый элемент в контейнере, для которого выполняется некое условие. В качестве параметров он принимает итератор на начало контейнера, итератор на конец и лямбда-функцию, аналогично алгоритму count_if. Воспользуйтесь алгоритмом find_if и найдите в заданном векторе языков программирования первый язык, начинающийся на “C”. Чтобы получить доступ к элементу контейнера через итератор, примените оператор *:
cout << *it << endl; 
Считайте, что искомый элемент в контейнере существует.
Пример вывода:
С# 

Чтобы правильно вызвать find_if, пригодятся лямбда-функции. Вернитесь к уроку о них, если возникли сложности.
*/

int main() {
    vector<string> langs = {"Python"s, "Java"s, "C#"s, "Ruby"s, "C++"s};
    /*string str = "C#"s;
    auto variavle = str[0]=='C' ;
    cout << str[0]<< variavle  << endl;*/
    // Выведите первый язык, начинающийся на C, используя алгоритм find_if
    
    auto is_tru = [](string i){ return i[0] == 'C'; };

    //auto result3 = find_if(begin(langs), end(langs), is_tru);
    auto result3 = find_if(langs.begin(), langs.end(), is_tru);
    cout <<result3 << endl;
}

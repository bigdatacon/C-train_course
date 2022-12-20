#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;
/*
Задание 1
Реализуйте функцию SortBy в соответствии с указанными требованиями. В процессе у вас могут возникнуть проблемы. Постарайтесь решить их самостоятельно: читайте подсказки.
Код должен выводить:
Мурка Белка Георгий Рюрик 
Белка Георгий Мурка Рюрик 
Мурка Георгий Рюрик Белка 

ПОДСКАЗКИ:
Чтобы написать компаратор, укажите типы параметров lhs и rhs. С ходу это сделать не получается, потому что известен только тип контейнера. Здесь поможет auto. Задайте список параметров const auto& lhs, const auto& rhs, и тогда сам компаратор окажется по сути шаблонной функцией.
Чтобы вызвать в компараторе-лямбде key_mapper, захватите его: [key_mapper](const auto& lhs, const auto& rhs).
*/


struct Animal {
    string name;
    int age;
    double weight;
};

template <typename Container, typename KeyMapper>
void SortBy(Container& container, KeyMapper key_mapper) {
    // теперь можно сортировать контейнер
    // с компаратором key_mapper(lhs) < key_mapper(rhs)
}

void PrintNames(const vector<Animal>& animals) {
    for (const Animal& animal : animals) {
        cout << animal.name << ' ';
    }
    cout << endl;
}

int main() {
    vector<Animal> animals = {
        {"Мурка"s, 10, 5},
        {"Белка"s, 5, 1.5},
        {"Георгий"s, 2, 4.5},
        {"Рюрик"s, 12, 3.1},
    };

    PrintNames(animals);

    SortBy(animals, [](const Animal& animal) {
        return animal.name;
    });
    PrintNames(animals);

    SortBy(animals, [](const Animal& animal) {
        return -animal.weight;
    });
    PrintNames(animals);

    return 0;
}

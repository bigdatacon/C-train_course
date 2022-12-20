Задание 2
Мы ловко упорядочили животных по убыванию веса, отсортировав по возрастанию -weight. Но с именем так не выйдет. Добавьте в функцию необязательный третий параметр reverse. Если туда будет передано true, объекты нужно отсортировать в обратном порядке.
Не используйте функцию reverse из библиотеки <algorithm>.
Пример
int main() {
    vector<Animal> animals = {
        {"Мурка"s,   10, 5},
        {"Белка"s,   5,  1.5},
        {"Георгий"s, 2,  4.5},
        {"Рюрик"s,   12, 3.1},
    };
    PrintNames(animals);
    SortBy(animals, [](const Animal& animal) { return animal.name; }, true);
    PrintNames(animals);
    SortBy(animals, [](const Animal& animal) { return animal.weight; });
    PrintNames(animals);
    return 0;
} 
Вывод
Мурка Белка Георгий Рюрик 
Рюрик Мурка Георгий Белка 
Белка Рюрик Георгий Мурка 

Здесь будет достаточно любого решения, даже с небольшим копированием кода. Например, можно начать функцию с if (reverse) и в каждой ветке отсортировать с нужным компаратором

#include <algorithm>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct Animal {
    string name;
    int age;
    double weight;
};

template<typename Container, typename KeyMapper>
void SortBy(Container &container, KeyMapper key_mapper) {
sort(container.begin(), container.end(), [key_mapper](const auto& lhs, const auto& rhs) {
			return key_mapper(lhs) < key_mapper(rhs);
		});
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

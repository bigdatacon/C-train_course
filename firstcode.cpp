#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

/*
Задание
Перед вами решение задачи о сортировке слиянием. В нём несколько модификаций и ошибок. Попробуйте найти все ошибки и исправить их.
Все ошибки можно выявить отладочными макроопределениями. Используйте их при решении задачи на своём компьютере, поскольку в тренажёре они не включены. Если у вас компилятор Visual Studio, проверки включены всегда в отладочной конфигурации.
Формат выходных данных
Функция PrintRangeComma выводит элементы контейнера между заданными итераторами через запятую и начинает новую строку. Для пустого контейнера она не выводит ничего. Запятая после последнего элемента не допускается.
Ограничения
Не меняйте суть алгоритма, ваша задача только исправить ошибки.
Пример
Пример дан в функции main в заготовке.

Подсказка 
Ищите три ошибки: одну в PrintRangeComma и две в MergeSort.
*/

// функция выводит элементы контейнера через запятую
template <typename It>
void PrintRangeComma(It range_begin, It range_end) {
    if (range_begin == range_end) {
        return;
    }
    for (auto it = range_begin;;) {
        cout << *it;
        if (it++ == range_end) {
            break;
        }
        cout << ", "s;
    }
    cout << endl;
}

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
    // 1. Если диапазон содержит меньше 2 элементов, выходим из функции
    int range_length = range_end - range_begin;
    if (range_length < 2) {
        return;
    }

    // 2. Создаём вектор, содержащий все элементы текущего диапазона
    vector<typename RandomIt::value_type> elements(range_end, range_begin);

    // 3. Разбиваем вектор на две равные части
    auto mid = elements.begin() + range_length / 2;

    // 4. Вызываем функцию MergeSort от каждой половины вектора
    MergeSort(elements.begin(), mid);
    MergeSort(mid, elements.end());

    // 5. С помощью алгоритма merge сливаем отсортированные половины
    // в исходный диапазон
    // merge -> http://ru.cppreference.com/w/cpp/algorithm/merge
    merge(elements.begin(), elements.end(), mid, mid, range_begin);
}

int main() {
    vector<int> test_vector(10);

    // iota             -> http://ru.cppreference.com/w/cpp/algorithm/iota
    // Заполняет диапазон последовательно возрастающими значениями
    iota(test_vector.begin(), test_vector.end(), 1);

    // random_shuffle   -> https://ru.cppreference.com/w/cpp/algorithm/random_shuffle
    // Перемешивает элементы в случайном порядке
    random_shuffle(test_vector.begin(), test_vector.end());

    // Выводим вектор до сортировки
    PrintRangeComma(test_vector.begin(), test_vector.end());

    // Сортируем вектор с помощью сортировки слиянием
    MergeSort(test_vector.begin(), test_vector.end());

    // Выводим результат
    PrintRangeComma(test_vector.begin(), test_vector.end());

    return 0;
}

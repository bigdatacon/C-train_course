#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

#include <random>
#include <algorithm>
#include <iostream>

using namespace std;

// g++ qww.cpp -o find -std=c++17 -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_LIBCPP_DEBUG=1
// через gdb

//g++ qww.cpp -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_LIBCPP_DEBUG=1 -g -o qww
//gdb qww
//gdb // break abort
//r
//bt
// после bt должна быть показана строка где ошибка


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

/*
 Что нашел отладчик:
 (gdb) bt
#0  0x00007ff97e49f1e7 in msvcrt!abort () from C:\Windows\System32\msvcrt.dll
#1  0x000000006fc62a11 in libstdc++-6!_ZNK11__gnu_debug16_Error_formatter8_M_errorEv () from C:\dev\mingw64\bin\libstdc++-6.dll
#2  0x0000000000405d15 in __gnu_debug::_Safe_iterator<__gnu_cxx::__normal_iterator<int*, std::__cxx1998::vector<int, std::allocator<int> > >, std::__debug::vector<int, std::allocator<int> > >::operator* (this=0x64fb30)
    at C:/dev/mingw64/lib/gcc/x86_64-w64-mingw32/8.1.0/include/c++/debug/safe_iterator.h:268
#3  0x000000000040312d in PrintRangeComma<__gnu_debug::_Safe_iterator<__gnu_cxx::__normal_iterator<int*, std::__cxx1998::vector<int, std::allocator<int> > >, std::__debug::vector<int, std::allocator<int> > > > (range_begin=9,
    range_end=-1414812757) at qww.cpp:34
#4  0x00000000004016ce in main () at qww.cpp:80
 * */

// функция выводит элементы контейнера через запятую
template <typename It>
void PrintRangeComma(It range_begin, It range_end) {
    if (range_begin == range_end) {
        return;
    }
    for (auto it = range_begin;;) {
        cout << *it;
        if (it++ == range_end -1) {  //добавил -1 чтобы не выходить за предлелы цикла
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
    //vector<typename RandomIt::value_type> elements(range_end, range_begin);
    vector<typename RandomIt::value_type> elements(range_begin, range_end); // поправил тут передал начало и конец

    // 3. Разбиваем вектор на две равные части
    auto mid = elements.begin() + range_length / 2;

    // 4. Вызываем функцию MergeSort от каждой половины вектора
    MergeSort(elements.begin(), mid);
    MergeSort(mid, elements.end());

    // 5. С помощью алгоритма merge сливаем отсортированные половины
    // в исходный диапазон
    // merge -> http://ru.cppreference.com/w/cpp/algorithm/merge
    //merge(elements.begin(), elements.end(), mid, mid, range_begin);
    merge(elements.begin(), mid, mid, elements.end(), range_begin);  // Вот так нужно но это из готоого решения
}

int main() {
    vector<int> test_vector(10);

    // iota             -> http://ru.cppreference.com/w/cpp/algorithm/iota
    // Заполняет диапазон последовательно возрастающими значениями
    iota(test_vector.begin(), test_vector.end(), 1);

    // random_shuffle   -> https://ru.cppreference.com/w/cpp/algorithm/random_shuffle
    // Перемешивает элементы в случайном порядке
    random_shuffle(test_vector.begin(), test_vector.end());
    //shuffle(test_vector.begin(), test_vector.end());

    // Выводим вектор до сортировки
    PrintRangeComma(test_vector.begin(), test_vector.end());

    // Сортируем вектор с помощью сортировки слиянием
    MergeSort(test_vector.begin(), test_vector.end());

    // Выводим результат
    PrintRangeComma(test_vector.begin(), test_vector.end());

    return 0;
}

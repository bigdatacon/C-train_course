#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>
#include <valarray>


using namespace std;
/*
Задание 2
Напишите функцию-шаблон MergeSort. Она должна принимать итераторы категории произвольного доступа, указывающие на начало и конец контейнера, который нужно отсортировать. Сортируйте по возрастанию. Считайте определённой операцию сравнения между элементами. 
Есть много разных способов сортировать, но в этой задаче реализуйте сортировку методом слияния. Алгоритм такой сортировки:
Разделить контейнер на две примерно равные части;
Отсортировать каждую из этих частей;
Объединить две части так, чтобы сортировка сохранилась.
Пункт два в свою очередь решается теми же шагами. Этот алгоритм может быть реализован как рекурсивно, так и итеративно (с применением цикла).
Не используйте стандартный алгоритм сортировки sort и всех его «родственников» — это будет слишком просто.

Предположим, наш диапазон состоит из двух элементов.
Разделите его пополам. В каждой части получилось по одному элементу.
Каждая из половин уже отсортирована, ведь она состоит всего из одного элемента.
Найдите подходящий алгоритм, который соединяет два отсортированных диапазона.
*/

/*template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end)*/

template <typename It>
void PrintRange(It range_begin, It range_end){
    for (auto it = range_begin; it != range_end; it++) {
            cout << *it << " "s;
            
        }
        cout << endl;
    }

template <typename It_Begin, typename It_End>
vector<int> SliceVec(It_Begin it_begin, It_End it_end)
{
    vector<int> vec;
    for (auto i = it_begin; i < it_end; ++i){
        vec.push_back(*i);
    }
        
	return vec;
}

vector<int> SliceVec2(vector<int> vector_input, int it_begin, int it_end)
{
    vector<int> vec;
    for (auto i = it_begin; i < it_end; ++i){
        vec.push_back(vector_input[i]);
    }
        
	return vec;
}


int main() {
    vector<int> test_vector(10);
    // iota             -> http://ru.cppreference.com/w/cpp/algorithm/iota
    // Заполняет диапазон последовательно возрастающими значениями
    iota(test_vector.begin(), test_vector.end(), 1);
    // random_shuffle   -> https://ru.cppreference.com/w/cpp/algorithm/random_shuffle
    // Перемешивает элементы в случайном порядке
    random_shuffle(test_vector.begin(), test_vector.end());
    vector<int> new_vector = SliceVec(test_vector.begin(), test_vector.end());
    vector<int> new_vector2 = SliceVec2(test_vector, 0, test_vector.size()/2);
    vector<int> new_vector3 = SliceVec2(test_vector, test_vector.size()/2, test_vector.size());
    // Выводим вектор до сортировки
    PrintRange(test_vector.begin(), test_vector.end());
    // Сортируем вектор с помощью сортировки слиянием
    //MergeSort(test_vector.begin(), test_vector.end());
    // Выводим результат
    //PrintRange(test_vector.begin(), test_vector.end());
    PrintRange(new_vector.begin(), new_vector.end());
    PrintRange(new_vector2.begin(), new_vector2.end());
    PrintRange(new_vector3.begin(), new_vector3.end());
    
    return 0;
}

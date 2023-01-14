#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>
#include <valarray>

using namespace std;

void bubbleSort(vector<int>& a)
{
      bool swapp = true;
      while(swapp){
        swapp = false;
        for (size_t i = 0; i < a.size()-1; i++) {
            if (a[i]>a[i+1] ){
                a[i] += a[i+1];
                a[i+1] = a[i] - a[i+1];
                a[i] -=a[i+1];
                swapp = true;
            }
        }
    }
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

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end){
    
    if (range_end == range_begin + 1) {
        return;
    }
    vector<int> vec; // определяю промежуточный вектора для соединения половинок 
    
    // Получаем количество элементов между итераорами
    int count = range_end - range_begin;
    // Находим середину контейнера
    auto range_middle = range_begin + count / 2;
    // создаю подвекторы 
    vector<int> v1 = SliceVec(range_begin, range_middle);
    vector<int> v2 = SliceVec(range_middle, range_end);
    bubbleSort(v1);   // сортирую подвекторы пузырьков 
    bubbleSort(v2);
    
    auto siz1 = v1.size();
    auto siz2 = v2.size();
    size_t p1 = 0;
    size_t p2 = 0;
    
    
    while (vec.size() < siz1 + siz2) {
        if (p1 < siz1 && v1.at(p1) < v2.at(p2)) {
            vec.push_back(v1.at(p1));
            ++p1;
        } else {
            vec.push_back(v2.at(p2));
            ++p2;
        }
    }

    
        // Копируем временный вектор в исходный
    for (auto it = range_begin; it != range_end; it++) {
        *it = vec[it - range_begin];
    }
}

template <typename It>
void PrintRange(It range_begin, It range_end){
    for (auto it = range_begin; it != range_end; it++) {
            cout << *it << " "s;
            
        }
        cout << endl;
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
    PrintRange(test_vector.begin(), test_vector.end());
    // Сортируем вектор с помощью сортировки слиянием
    MergeSort(test_vector.begin(), test_vector.end());
    // Выводим результат
    PrintRange(test_vector.begin(), test_vector.end());
    return 0;
}

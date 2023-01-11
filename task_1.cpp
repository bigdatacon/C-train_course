// заготовка

#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>
#include <string>
using namespace std;
 
template <typename RandomIt>
void MergeSort(RandomIt  begin, RandomIt end) {
 
    if ( begin < end ){
        swap(*(min_element(begin, end)), *begin);
        swap(*(max_element(begin, end)), *(end - 1));
    } else{
        vector<typename RandomIt::value_type> elements(begin, end);
        auto r_e = elements.begin() + elements.size() / 2;
        MergeSort(begin, r_e);
        MergeSort(r_e, end);
        merge(begin, r_e, r_e, end, begin);
    }
}
template <typename A>
void PrintRange(A it_1, A it_2){
    while (it_1 != it_2){
       cout << *it_1 << " ";
       it_1++; 
    }
    cout << endl;
}
 
int main() {
    vector<int> test_vector(3);
 
    // iota             -> http://ru.cppreference.com/w/cpp/algorithm/iota
    // Заполняет диапазон последовательно возрастающими значениями
    iota(test_vector.begin(), test_vector.end(), 1);
    reverse(test_vector.begin(), test_vector.end());
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
 

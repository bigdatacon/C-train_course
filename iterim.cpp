#include <cassert>
#include <vector>
#include <iostream>

using namespace std;

/*
адание
Напишите шаблонную функцию ReverseArray, которая заменяет порядок элементов массива на противоположный. Функция принимает адрес первого элемента массива и количество элементов массива.
Сигнатура функции ReverseArray:
template <typename T>
void ReverseArray(T* start, size_t size); 
Переданный массив может быть пустым или непустым. В случае пустого массива параметр size равен нулю, а указатель start может быть как нулевым, так и ненулевым указателем. Если массив непустой, указатель start будет ненулевым.
Ограничения
Функция ReverseArray должна использовать O(1)O(1) дополнительной памяти, то есть её объём не должен зависеть от размера массива. Например, нельзя использовать vector или иной контейнер для промежуточного хранения элементов. Ограничение по времени выполнения — O(N)O(N).

Корректно обрабатывайте вырожденные случаи — пустой массив и массив из одного элемента. Убедитесь, что программа правильно работает с массивами чётного и нечётного размеров.
*/

template <typename T>
void ReverseArray(T* start, size_t size) {
    cout << "before reverse : "s << endl;
    for (auto i = start; i < (start+ size); ++i ){
        
        cout <<  *i << ", "s;}
    cout <<  endl;
        //cout << endl;
    
    // Напишите тело функции самостоятельно
    if (size == 0 || size == 1) {return;} // проверяю массив на пустоту
    else {
    
    int minus = size-1;
    for (auto i = start; i < (start+ size); ++i ){
        //cout <<"before i : " <<  *i<< endl;
        auto tmp = *i;

        
        cout <<"before i : " <<  *i<< " before *i+minus : " <<  *(i+minus)<< " minus : " << minus << "  tmp : "<< tmp <<    endl;
        *i = *(i+minus);
        *(i+minus) =  tmp ;
        minus-=2;
        cout <<"after i : " <<  *i<<  " *i+minus : "<< *(i+minus) << " minus : " << minus <<   endl;
        
        //cout <<"after *i+minus : " <<  *(i+minus)<< " minus : " << minus <<   endl;
        
    }
    }
    cout << "final : "s << endl;
    for (auto i = start; i < (start+ size); ++i )
        cout <<  *i << ", ";
    
}

int main() {
    using namespace std;
    
    //vector<int> v = {1, 2};
    vector<int> v = {1, 2,3 , 5 };
    ReverseArray(v.data(), v.size());
    //assert(v == (vector<int>{2, 1}));
}

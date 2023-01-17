#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>

using namespace std;
/*
Задание 1
Удобно, когда IsEmpty не пересчитывает все элементы, а другим способом отвечает, пуст стек или нет. В этом задании вы сделаете подобный трюк. Напишите реализацию класса StackMin, у которого кроме стандартных методов класса Stack будет дополнительный метод PeekMin. Он должен уметь вернуть минимальный по значению на данный момент элемент в стеке. Важно организовать стек так, чтобы для поиска минимума не пришлось перебирать все элементы в стеке. Перебора элементов не должно быть ни в методе PeekMin, ни в каком другом. Возьмите эту заготовку класса и дополните её своей реализацией:


Подсказка 
Когда нужно что-то вернуть без пересчётов и вычислений, обычно эту информацию нужно где-то хранить. Просто добавить в private член класса minimum будет недостаточно. Посмотрим на такой случай:
Добавим в стек 1. minimum_ станет равен 1.
Добавим в стек 5. minimum_ останется равен 1, так как 1 < 5.
Добавим в стек 1. minimum_ всё ещё 1.
Добавим в стек 2. minimum_ также остался 1.
Вытащим из стека 2. Ничего не поменялось.
Вытащим из стека 1. Стек ещё не пуст, в нём остались 1 и 5. Но теперь не понять, чему равен минимум, не вытаскивая все элементы из стека и не перебирая их.
Придумаем, как выбраться из тупика. Вероятно, хранить нужно не один последний минимальный элемент. Если удаляем элемент из стека, нужно знать, какой теперь минимум. Было бы здорово для каждого элемента в стеке знать, чему равен минимум, если этот элемент — вершина стека. Тогда алгоритм начинает работать так:
Добавим в стек 1. Минимум в этом случае равен самому элементу. Нужно сохранить эту пару: элемент 1 — минимум 1.
Добавим в стек 5. Минимум остался равен 1. Сохраняем: элемент 5 — минимум 1.
Добавим в стек 1. Минимум тот же. Сохраняем: элемент 1 — минимум 1.
Добавляем 2. Сохраняем: элемент 2 — минимум 1.
Вытаскиваем 2. На вершине оказалось «элемент 1 — минимум 1». И нет проблем дальше вытаскивать следующий элемент и отвечать на запрос про текущий минимум.


Вывод: 
5 2 4 3 1
Минимум = 1
5 2 4 3
Минимум = 2
5 2 4
Минимум = 2
5 2
Минимум = 2
5
Минимум = 5
*/

template <typename It>
void PrintRange(It range_begin, It range_end) {
    for (auto it = range_begin; it != range_end; ++it) {
        cout << *it << " "s;
    }
    cout << endl;
}

template <typename Type>
class Stack {
public:
    void Push(const Type& element) {
        elements_.push_back(element);
    }
    void Pop() {
        // напишите реализацию
        elements_.pop_back();
    }
    const Type& Peek() const {
        //return elements_;
        return elements_.back();
        // напишите реализацию
    }
    Type& Peek() {
        // напишите реализацию
        //return elements_;
        return elements_.back();
    }
    void Print() const {
        // напишите реализацию
        for (auto el : elements_){cout << el << " ";}
        cout <<  endl;
    }
    uint64_t Size() const {
        // напишите реализацию
        return elements_.size();
    }
    bool IsEmpty() const {
        // напишите реализацию
        return elements_.empty();
    }

private:
    vector<Type> elements_;
};

template <typename Type>
class StackMin {
public:
    void Push(const Type& element) {
    // напишите реализацию метода
    //elements_.push_back(element);
    elements_.Push(element);
    bool new_bigger = element> max_element_;
    if (new_bigger){elements_maximums_.emplace(element, element); max_element_ = element; }
    else {elements_maximums_.emplace(element, max_element_);};
    
    }
    void Pop() {
    // напишите реализацию метода
    //elements_.pop_back();
    elements_.Pop();
    }
    const Type& Peek() const {
        return elements_.Peek();
    }
    Type& Peek() {
        return elements_.Peek();
    }
    void Print() const {
        // работу этого метода мы проверять не будем,
        // но если он вам нужен, то можете его реализовать
        elements_.Print();
    }
    uint64_t Size() const {
        return elements_.size();
    }
    bool IsEmpty() const {
        return elements_.IsEmpty();
    }
    const Type& PeekMin() const {
    // напишите реализацию метода
        auto iter_end = elements_maximums_.end();
        return iter_end -> second;
    }
    Type& PeekMin() {
    // напишите реализацию метода
         auto iter_end = elements_maximums_.end();
        return iter_end -> second;
    }
private:
    Stack<Type> elements_;
    Type max_element_ = 0;  // задаю начальный максимум нулем
    //Type max_element_ ;  // задаю начальный максимум нулем
    map<Type, Type> elements_maximums_;
    
    
    // возможно, здесь вам понадобится что-то изменить
};

int main() {
    StackMin<int> stack;
    vector<int> values(5);
    // заполняем вектор для тестирования нашего стека
    iota(values.begin(), values.end(), 1);
    // перемешиваем значения
    random_shuffle(values.begin(), values.end());
    // заполняем стек
    for (int i = 0; i < 5; ++i) {
        stack.Push(values[i]);
    }
    // печатаем стек и его минимум, постепенно убирая из стека элементы
    while (!stack.IsEmpty()) {
        stack.Print();
        cout << "Минимум = "s << stack.PeekMin() << endl;
        stack.Pop();
    }
}

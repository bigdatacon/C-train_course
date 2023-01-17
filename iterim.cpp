#include <stack>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

/*
Задание 1
Внутри контейнера queue в стандартной библиотеке находится deque. Класс queue даёт доступ только к определённым возможностям класса deque. Вам реализовывать класс queue с применением deque не нужно. Сделайте свою реализацию, используя два стека.
Вы спросите, зачем заниматься такой ерундой. Вспомним задачу о нахождении минимума среди элементов стека без перебора. Структура стека позволяет сделать это достаточно легко. Но структура deque так не сможет. Поэтому, если для очереди понадобится поддержка поиска минимума или максимума без перебора элементов, реализация с применением стеков — самая подходящая.
Напишите реализацию методов класса Queue, отвечающих принципу FIFO, если внутри контейнера два стека.
Внимание, ваша очередь имеет лишь неконстантную версию метода Front. В этом задании для доступа к первому элементу понадобится переместить элементы из одного стека в другой, а в константном методе это сделать проблематично. Язык C++ в некоторых случаях разрешает константным методам изменять поля класса. Эту возможность рассмотрим в будущих уроках.

ВЫВОД:
5 2 4 3 1
Заполняем очередь
Вставленный элемент 5
Первый элемент очереди 5
Вставленный элемент 2
Первый элемент очереди 5
Вставленный элемент 4
Первый элемент очереди 5
Вставленный элемент 3
Первый элемент очереди 5
Вставленный элемент 1
Первый элемент очереди 5
Вынимаем элементы из очереди
Будем вынимать элемент 5
Будем вынимать элемент 2
Будем вынимать элемент 4
Будем вынимать элемент 3
Будем вынимать элемент 1

ПОДСКАЗКА:
Посмотрите на вставку элемента. Сначала вариантов немного: просто складывайте элементы в первый стек. Когда накопится несколько элементов, вы хотите что-нибудь оттуда вытащить. По принципу FIFO первым должен выйти тот элемент, который сейчас находится на дне первого стека. Нужно поправить эту ситуацию. Переложите все элементы первого стека во второй.
*/

using namespace std;

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
        elements_.pop_back();
    }
    const Type& Peek() const {
        return elements_.back();
    }
    Type& Peek() {
        return elements_.back();
    }
    void Print() const {
        PrintRange(elements_.begin(), elements_.end());
    }
    uint64_t Size() const {
        return elements_.size();
    }
    bool IsEmpty() const {
        return elements_.empty();
    }

private:
    vector<Type> elements_;
};

template <typename Type>
class Queue {
public:
    void Push(const Type& element) {
        // напишите реализацию
        //in_stack_.Push(element);   // ни так ни так не работает 
        in_stack_.push_back(element);
    }
    void Pop() {
        // напишите реализацию
        if (!out_stack_.IsEmpty()) {
            out_stack_.Pop();
        }
        else {
            for (auto el : in_stack_) { out_stack_.Push(el); }  // добавляю элементы в outstack 
            out_stack_.Pop(); // беру последний элемент из outstack 
        }
    }
    Type& Front() {
        // напишите реализацию
        return Pop();
    }
    uint64_t Size() const {
        // напишите реализацию
        return in_stack_.Size();
    }
    bool IsEmpty() const {
        return in_stack_.Empty();
    }

private:
    stack<Type> in_stack_;
    stack<Type> out_stack_;
};

int main() {
    Queue<int> queue;
    vector<int> values(5);
    // заполняем вектор для тестирования очереди
    iota(values.begin(), values.end(), 1);
    // перемешиваем значения
    random_shuffle(values.begin(), values.end());
    PrintRange(values.begin(), values.end());
    cout << "Заполняем очередь"s << endl;
    // заполняем очередь и выводим элемент в начале очереди
    for (int i = 0; i < 5; ++i) {
        queue.Push(values[i]);
        cout << "Вставленный элемент "s << values[i] << endl;
        cout << "Первый элемент очереди "s << queue.Front() << endl;
    }
    cout << "Вынимаем элементы из очереди"s << endl;
    // выводим элемент в начале очереди и вытаскиваем элементы по одному
    while (!queue.IsEmpty()) {
        // сначала будем проверять начальный элемент, а потом вытаскивать,
        // так как операция Front на пустой очереди не определена
        cout << "Будем вынимать элемент "s << queue.Front() << endl;
        queue.Pop();
    }
    return 0;
}

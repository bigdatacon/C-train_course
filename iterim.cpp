#include <iostream>
#include <vector>

using namespace std;
/*
Задание 1
Чтобы работать со стеком по всем правилам LIFO, напишите класс, который поможет это сделать. Храните элементы стека в векторе. Напишите свою реализацию методов Push, Pop, Peek, Size и IsEmpty. Для удобства добавьте ещё метод Print. Элементы могут быть разных типов, поэтому нужен класс-шаблон. 
Методов Peek два, потому что один из них будет вызываться в случае константного объекта, а другой — в остальных случаях. При этом возвращаемое значение будет разным. Если объект константный, нужно гарантировать, что пользователь не изменит тот объект, на который ему передаётся ссылка. Поэтому в константном методе возвращается const Type&. В другом случае можно спокойно вернуть ссылку без const, так как стек не будет неизменяемым объектом — пользователь может свободно его менять. Этот же принцип будет выполняться всегда, когда предоставляете пользователю доступ к элементам контейнера.


Подсказка 
Добавление и удаление в вектор должны происходить с одного конца. Удобнее сделать это именно с конца, а не с начала. Примените стандартные методы контейнера vector.

Вывод: 
0
0 1
0 1 2
0 1 2 3
0 1 2 3 4
0 1 2 3 4 5
0 1 2 3 4 5 6
0 1 2 3 4 5 6 7
0 1 2 3 4 5 6 7 8
0 1 2 3 4 5 6 7 8 9
0 1 2 3 4 5 6 7 8
0 1 2 3 4 5 6 7
0 1 2 3 4 5 6
0 1 2 3 4 5
0 1 2 3 4
0 1 2 3
0 1 2
0 1
0
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
        return elements_;
        // напишите реализацию
    }
    Type& Peek() {
        // напишите реализацию
        return elements_;
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


int main() {
    Stack<int> stack;
    for (uint32_t i = 0; i < 10; ++i) {
        stack.Push(i);
        stack.Print();
    }
    while (!stack.IsEmpty()) {
        stack.Pop();
        stack.Print();
    }
}

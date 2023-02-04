#include <cassert>
#include <cstddef>
#include <string>
#include <utility>

/*
Задание
Создайте шаблонный класс односвязного списка SingleLinkedList<Type> со следующим функционалом:
конструктор по умолчанию, который создаёт пустой список;
метод GetSize, который возвращает количество элементов в списке;
метод IsEmpty, который возвращает true, если список пустой, и false в противном случае.

Подсказка
Односвязный список считается пустым, только если в нём ноль элементов.
Не мудрите — в этом задании нужно всего лишь реализовать пустой односвязный список.
Если явно заданных конструкторов нет, компилятор сам сгенерирует конструктор по умолчанию. Не забудьте задать корректные значения полям класса.
*/

/*
Односвязный список допускает следующие операции:
вставка элемента в начало или конец списка,
вставка элемента после некоторого элемента списка,
удаление элемента, следующего за данным элементом списка,
проверка списка на пустоту,
определение количества элементов в списке.
*/

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };

public:
    // Возвращает количество элементов в списке за время O(1)
    [[nodiscard]] size_t GetSize() const noexcept {
        // Заглушка. Реализуйте метод самостоятельно
        // Возможно вместо this нужно использовать Node
            int size = 0;
            if (this->Next != NULL)
            {
                size += 1;
                GetSize(this->Next);//переход к следующему элементу}
            }
            return size;

        
        assert(false);
        return 42;
    }

    // Сообщает, пустой ли список за время O(1)
    [[nodiscard]] bool IsEmpty() const noexcept {
        // Заглушка. Реализуйте метод самостоятельно
        return this->head_ == NULL;  // так нашел в интернете 
        
        assert(false);
        return false;
    }

private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_;
    size_t size_;
};

void Test0() {
    using namespace std;
    {
        const SingleLinkedList<int> empty_int_list;
        assert(empty_int_list.GetSize() == 0u);
        assert(empty_int_list.IsEmpty());
    }

    {
        const SingleLinkedList<string> empty_string_list;
        assert(empty_string_list.GetSize() == 0u);
        assert(empty_string_list.IsEmpty());
    }
}

int main() {
    Test0();
}

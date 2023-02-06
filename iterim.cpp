
#include <cstddef>
#include <iterator>
#include <memory>
#include <sstream>
#include <type_traits>
#include <cassert>
#include <iostream>
#include <utility>
#include <algorithm>
#include <string_view>

using namespace std;


template <typename Type>
class SingleLinkedList {
  struct Node {
    Node() = default;
    Node(const Type& val, Node* next)
      : value(val)
      , next_node(next) {
    }
    Type value;
    Node* next_node = nullptr;
  };


  template <typename ValueType>
  class BasicIterator {
    friend class SingleLinkedList;

    explicit BasicIterator(Node* node) {
      node_ = node;
    }

  public:

    using iterator_category = std::forward_iterator_tag;
    using value_type = Type;
    using difference_type = std::ptrdiff_t;
    using pointer = ValueType*;
    using reference = ValueType&;

    BasicIterator() = default;

    BasicIterator(const BasicIterator<Type>& other) noexcept {
      node_ = other.node_;
    }

    BasicIterator(const BasicIterator<const Type>& other) noexcept {
      node_ = other.node_;
    }

    BasicIterator& operator=(const BasicIterator<ValueType>& rhs) = default;

    [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
      return (node_ == rhs.node_);
    }

    [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
      return (node_ != rhs.node_);
    }

    [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
      return (node_ == rhs.node_);
    }

    [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
      return (node_ != rhs.node_);
    }

    BasicIterator& operator++() noexcept {
      if (node_) { node_ = node_->next_node; };
      return *this;
    }

    BasicIterator operator++(int) noexcept {
      auto old_value(*this); 
      if (node_) { node_ = node_->next_node; };
      return old_value;      
    }

    [[nodiscard]] reference operator*() const noexcept {
      return node_->value;
    }

    [[nodiscard]] pointer operator->() const noexcept {
      return &node_->value;
    }

  private:
    Node* node_ = nullptr;
  };

public:
  Node* first = nullptr;
  Node* last = nullptr;
  SingleLinkedList() : first(nullptr) {}

  ~SingleLinkedList() {
    Clear();
  }
  using value_type = Type;
  using reference = value_type&;
  using const_reference = const value_type&;

  using Iterator = BasicIterator<Type>;
  using ConstIterator = BasicIterator<const Type>;

  [[nodiscard]] Iterator begin() noexcept {
    return Iterator(first);
  }

  [[nodiscard]] Iterator end() noexcept {
    return Iterator(nullptr);
  }

  [[nodiscard]] ConstIterator begin() const noexcept {
    return ConstIterator(first); 
  }

  [[nodiscard]] ConstIterator end() const noexcept {
    return ConstIterator(nullptr);
  }

  [[nodiscard]] ConstIterator cbegin() const noexcept {
    return ConstIterator(first); 
  }

  [[nodiscard]] ConstIterator cend() const noexcept {
    return ConstIterator(nullptr); 
  }
  
  SingleLinkedList(std::initializer_list<Type> values) {
       
        SingleLinkedList tmp;
        for (auto item: values) {
            tmp.PushBack(item);
        }

        swap(tmp);   
    }

    SingleLinkedList(const SingleLinkedList& other) {

        SingleLinkedList tmp;
        for (auto item : other) {
            tmp.PushBack(item);
        }
        swap(tmp);
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
       if (this != &rhs){
            SingleLinkedList tmp;
            for (auto item : rhs) {
                tmp.PushBack(item);
            }
            swap(tmp);
       }
       return *this;
    }

    void swap(SingleLinkedList& other) noexcept {
        Node * tmp = other.first;
        other.first = first;
        first = tmp;
        tmp = other.last;
        other.last = last;
        last = tmp;
    }

  [[nodiscard]] size_t GetSize() const noexcept {
    int size = 0;
    Node* p = first;
    while (p) {
      size += 1;
      p = p->next_node;
    }
    return size;

  }

  [[nodiscard]] bool IsEmpty() const noexcept {
    return first == nullptr;  
  }

  void PushFront(const Type& value) {
    first = new Node(value, first);
    if (!last) {
        last = first;
    }
    ++size_;
  }

  void PushBack(const Type& value) {
    if (last) {
        last->next_node = new Node(value, nullptr);
        last = last->next_node;
    } else {
        first = new Node(value, first);
        last = first;
    }
  }

  void Clear() noexcept {
    while (first) {
      Node* next = first->next_node;
      delete first;
      first = next;
    }
    first = nullptr;
    last = nullptr;
  }
  
  // Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] Iterator before_begin() noexcept {
        // Реализуйте самостоятельно
        return Iterator(&head_);
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        // Реализуйте самостоятельно
        return ConstIterator(&head_);
    }

    // Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
    // Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        // Реализуйте самостоятельно
        return ConstIterator(&head_);
    }

    /*
     * Вставляет элемент value после элемента, на который указывает pos.
     * Возвращает итератор на вставленный элемент
     * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
     */
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        // Заглушка. Реализуйте метод самостоятельно
        //Node* next = first->next_node;
        for (ConstIterator it = head_ ; it != last; it++) {
                if (it->next_node == pos){
                // итератора 
                node_base* my_node = pos;
                std::unique_ptr<node_base> old_next = std::move(my_node->next_node);
                std::unique_ptr<node<T>> new_node = std::make_unique<node<T>>(std::move(old_next), std::move(value));
                my_node->next = std::move(new_node);
                return my_node->next.get();
                
                }
            }
        
    }

    void PopFront() noexcept {
        // Реализуйте метод самостоятельно
          Node* second_el_node = first->next_node;  // нахожу второй элемент
          delete first;   // удаляю первый 
          first = second_el_node; // второй становится первым 
    }
    }

    /*
     * Удаляет элемент, следующий за pos.
     * Возвращает итератор на элемент, следующий за удалённым
     */
    Iterator EraseAfter(ConstIterator pos) noexcept {
        // Заглушка. Реализуйте метод самостоятельно
        return {};
    }
  
  
private:
  // Фиктивный узел, используется для вставки "перед первым элементом"
  Node head_;
  size_t size_;
};


// Эта функция проверяет работу класса SingleLinkedList
void Test4() {
    struct DeletionSpy {
        ~DeletionSpy() {
            if (deletion_counter_ptr) {
                ++(*deletion_counter_ptr);
            }
        }
        int* deletion_counter_ptr = nullptr;
    };

    // Проверка PopFront
    {
        SingleLinkedList<int> numbers{3, 14, 15, 92, 6};
        numbers.PopFront();
        assert((numbers == SingleLinkedList<int>{14, 15, 92, 6}));

        SingleLinkedList<DeletionSpy> list;
        list.PushFront(DeletionSpy{});
        int deletion_counter = 0;
        list.begin()->deletion_counter_ptr = &deletion_counter;
        assert(deletion_counter == 0);
        list.PopFront();
        assert(deletion_counter == 1);
    }

    // Доступ к позиции, предшествующей begin
    {
        SingleLinkedList<int> empty_list;
        const auto& const_empty_list = empty_list;
        assert(empty_list.before_begin() == empty_list.cbefore_begin());
        assert(++empty_list.before_begin() == empty_list.begin());
        assert(++empty_list.cbefore_begin() == const_empty_list.begin());

        SingleLinkedList<int> numbers{1, 2, 3, 4};
        const auto& const_numbers = numbers;
        assert(numbers.before_begin() == numbers.cbefore_begin());
        assert(++numbers.before_begin() == numbers.begin());
        assert(++numbers.cbefore_begin() == const_numbers.begin());
    }

    // Вставка элемента после указанной позиции
    {  // Вставка в пустой список
        {
            SingleLinkedList<int> lst;
            const auto inserted_item_pos = lst.InsertAfter(lst.before_begin(), 123);
            assert((lst == SingleLinkedList<int>{123}));
            assert(inserted_item_pos == lst.begin());
            assert(*inserted_item_pos == 123);
        }

        // Вставка в непустой список
        {
            SingleLinkedList<int> lst{1, 2, 3};
            auto inserted_item_pos = lst.InsertAfter(lst.before_begin(), 123);

            assert(inserted_item_pos == lst.begin());
            assert(inserted_item_pos != lst.end());
            assert(*inserted_item_pos == 123);
            assert((lst == SingleLinkedList<int>{123, 1, 2, 3}));

            inserted_item_pos = lst.InsertAfter(lst.begin(), 555);
            assert(++SingleLinkedList<int>::Iterator(lst.begin()) == inserted_item_pos);
            assert(*inserted_item_pos == 555);
            assert((lst == SingleLinkedList<int>{123, 555, 1, 2, 3}));
        };
    }

    // Вспомогательный класс, бросающий исключение после создания N-копии
    struct ThrowOnCopy {
        ThrowOnCopy() = default;
        explicit ThrowOnCopy(int& copy_counter) noexcept
            : countdown_ptr(&copy_counter) {
        }
        ThrowOnCopy(const ThrowOnCopy& other)
            : countdown_ptr(other.countdown_ptr)  //
        {
            if (countdown_ptr) {
                if (*countdown_ptr == 0) {
                    throw std::bad_alloc();
                } else {
                    --(*countdown_ptr);
                }
            }
        }
        // Присваивание элементов этого типа не требуется
        ThrowOnCopy& operator=(const ThrowOnCopy& rhs) = delete;
        // Адрес счётчика обратного отсчёта. Если не равен nullptr, то уменьшается при каждом копировании.
        // Как только обнулится, конструктор копирования выбросит исключение
        int* countdown_ptr = nullptr;
    };

    // Проверка обеспечения строгой гарантии безопасности исключений
    {
        bool exception_was_thrown = false;
        for (int max_copy_counter = 10; max_copy_counter >= 0; --max_copy_counter) {
            SingleLinkedList<ThrowOnCopy> list{ThrowOnCopy{}, ThrowOnCopy{}, ThrowOnCopy{}};
            try {
                int copy_counter = max_copy_counter;
                list.InsertAfter(list.cbegin(), ThrowOnCopy(copy_counter));
                assert(list.GetSize() == 4u);
            } catch (const std::bad_alloc&) {
                exception_was_thrown = true;
                assert(list.GetSize() == 3u);
                break;
            }
        }
        assert(exception_was_thrown);
    }

    // Удаление элементов после указанной позиции
    {
        {
            SingleLinkedList<int> lst{1, 2, 3, 4};
            const auto& const_lst = lst;
            const auto item_after_erased = lst.EraseAfter(const_lst.cbefore_begin());
            assert((lst == SingleLinkedList<int>{2, 3, 4}));
            assert(item_after_erased == lst.begin());
        }
        {
            SingleLinkedList<int> lst{1, 2, 3, 4};
            const auto item_after_erased = lst.EraseAfter(lst.cbegin());
            assert((lst == SingleLinkedList<int>{1, 3, 4}));
            assert(item_after_erased == (++lst.begin()));
        }
        {
            SingleLinkedList<int> lst{1, 2, 3, 4};
            const auto item_after_erased = lst.EraseAfter(++(++lst.cbegin()));
            assert((lst == SingleLinkedList<int>{1, 2, 3}));
            assert(item_after_erased == lst.end());
        }
        {
            SingleLinkedList<DeletionSpy> list{DeletionSpy{}, DeletionSpy{}, DeletionSpy{}};
            auto after_begin = ++list.begin();
            int deletion_counter = 0;
            after_begin->deletion_counter_ptr = &deletion_counter;
            assert(deletion_counter == 0u);
            list.EraseAfter(list.cbegin());
            assert(deletion_counter == 1u);
        }
    }
}

int main() {
    Test4();
} 

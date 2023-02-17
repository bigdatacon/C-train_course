#pragma once


#include <cassert>
#include <initializer_list>
#include "array_ptr.h"
#include <algorithm>
#include <stdexcept>


class ReserveProxyObj{
    public:
        ReserveProxyObj(size_t i):capacity_to_reserve(i){


        }
        size_t capacity_to_reserve;
    };


template<typename Type>
class SimpleVector {
public:




    using Iterator = Type;
    using ConstIterator = const Type;
    using pointer = ArrayPtr<Type>;


    SimpleVector() noexcept = default;


    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    //инициализация значениями по умолчанию происходит в ArrayPtr


    explicit SimpleVector(sizet size) :
            ptr(size), size(size), capacity(size) {


    }


    // Создаёт вектор из size элементов, инициализированных значением value


    SimpleVector(sizet size, const Type &value) :
            ptr(size), size(size), capacity(size) {
        for (sizet i = 0; i < size; ++i) {
            ptr[i] = value;
        }
        // Напишите тело конструктора самостоятельно
    }


    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializerlist<Type> init) :
            ptr(init.size()), size(init.size()), capacity(init.size()) {
        sizet i = 0;
        for (const auto &value : init) {
            ptr[i] = value;
            ++i;
        }
    }


    SimpleVector(const SimpleVector &other) :
            ptr(other.size), size(other.size), capacity(other.capacity) {
        // Напишите тело конструктора самостоятельно
        std::copy(other.begin(), other.end(), begin());


    }


    SimpleVector(ReserveProxyObj obj){
        Reserve(obj.capacity_to_reserve);
    }


    SimpleVector& operator=(const SimpleVector &rhs) {
        // Напишите тело конструктора самостоятельно
        delete[] ptr_.Release();
        pointer newptr(rhs.size);
        std::copy(rhs.begin(), rhs.end(), newptr.Get());
        ptr.swap(newptr);
        size = rhs.size;
        capacity = size_;
        return *this;
    }


    void Reserve(size_t new_capacity) {
        if (newcapacity <= capacity) {
            return;
        }
        SimpleVector<Type> swap_ptr(new_capacity);
        std::copy(begin(), end(), swapptr.begin());
        capacity = newcapacity;
        ptr.swap(swapptr.ptr);
    }


    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type &item) {
        if (size < capacity) {
            ptr[size] = item;
            ++size;
            return;
        }
        Resize(size + 1);
        ptr[size - 1] = item;
    }


    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type &value) {
            if(pos == end()){
            PushBack(value);
            return end() - 1;
            }
            if (size < capacity) {
                std::copybackward((Iterator)pos, end(), begin() + size);
                *((Iterator)pos) = value;
                ++size_;
                return (Iterator)pos;
            }


            SimpleVector<Type> swapptr((2 * capacity));
            std::copy(begin(), (Iterator)pos, swap_ptr.begin());
            std::copy((Iterator)pos,end(),swap_ptr.begin() + ((Iterator)pos - begin() + 1));
            auto return_it = swap_ptr.begin() + (pos - begin());
            *returnit = value;
            capacity = 2 * capacity;
            ++size;
            ptr_.swap(swapptr.ptr);
            return return_it;
        }


    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if (size > 0) {
            --size;
        }
    }


    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        // Напишите тело самостоятельно
        auto ret_it = (Iterator) pos;
        auto it_after = pos + 1;
        for (; it_after != end(); ++pos, ++it_after) {
            *(Iterator) pos = *itafter;
        }
        --size;
        return ret_it;
    }


    // Обменивает значение с другим вектором
    void swap(SimpleVector &other) noexcept {
        ptr.swap(other.ptr);
        std::swap(size, other.size);
        std::swap(capacity, other.capacity);


    }


    // Возвращает количество элементов в массиве
    sizet GetSize() const noexcept {
        // Напишите тело самостоятельно
        return size;
    }


    // Возвращает вместимость массива
    sizet GetCapacity() const noexcept {
        // Напишите тело самостоятельно
        return capacity;
    }


    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {


        return size_ ? false : true;
    }


    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {


        return ptr_[index];
    }


    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](sizet index) const noexcept {
        return ptr[index];
    }


    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(sizet index) {
        if (index >= size) {
            throw std::out_ofrange(" ");
        }
        return ptr[index];
    }


    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(sizet index) const {
        // Напишите тело самостоятельно
        if (index >= size) {
            throw std::out_ofrange(" ");
        }
        return ptr[index];
    }


    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {


        size_ = 0;
    }


    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        // Напишите тело самостоятельно
        if (newsize < size) {
            size_ = new_size;
            return;
        }
        if (newsize > size && newsize <= capacity) {
            std::fill(end(), end() + (newsize - size), Type { });
            size_ = new_size;
            return;
        }
        if (newsize > size && newsize > capacity) {
            SimpleVector<Type> swap_ptr(
                    (newsize > (2 * capacity) ? newsize : (2 * capacity)));
            std::copy(begin(), end(), swapptr.begin());
            size = newsize;
            capacity =
                    (newsize > (2 * capacity) ? newsize : (2 * capacity));
            ptr_.swap(swapptr.ptr);
        }
    }


    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return ptr_.Get();
    }


    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return ptr.Get() + size;
    }


    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        // Напишите тело самостоятельно
        return ptr_.Get();
    }


    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        // Напишите тело самостоятельно
        return ptr.Get() + size;
    }


    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        // Напишите тело самостоятельно
        return ptr_.Get();
    }


    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        // Напишите тело самостоятельно
        return ptr.Get() + size;
    }


private:
    pointer ptr_ { };
    sizet size { };
    sizet capacity { };
};


template<typename Type>
inline bool operator==(const SimpleVector<Type> &lhs,
        const SimpleVector<Type> &rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}


template<typename Type>
inline bool operator!=(const SimpleVector<Type> &lhs,
        const SimpleVector<Type> &rhs) {
    // Заглушка. Напишите тело самостоятельно
    return !(lhs == rhs);
}


template<typename Type>
inline bool operator<(const SimpleVector<Type> &lhs,
        const SimpleVector<Type> &rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
            rhs.end());
}


template<typename Type>
inline bool operator<=(const SimpleVector<Type> &lhs,
        const SimpleVector<Type> &rhs) {
    // Заглушка. Напишите тело самостоятельно
    return lhs < rhs || lhs == rhs;
}


template<typename Type>
inline bool operator>(const SimpleVector<Type> &lhs,
        const SimpleVector<Type> &rhs) {
    // Заглушка. Напишите тело самостоятельно
    return !(lhs < rhs);
}


template<typename Type>
inline bool operator>=(const SimpleVector<Type> &lhs,
        const SimpleVector<Type> &rhs) {
    // Заглушка. Напишите тело самостоятельно
    return !(lhs < rhs) || lhs == rhs;
}


ReserveProxyObj Reserve(size_t capacity_to_reserve) {


    return ReserveProxyObj(capacity_to_reserve);
};

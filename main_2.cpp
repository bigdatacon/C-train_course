#pragma once
 
#include <bits/stdc++.h>
#include "array_ptr.h"
 
template<typename Type>
class SimpleVector {
public:
    using Iterator = ArrayPtr<Type>;
    using ConstIterator = const ArrayPtr<Type>;
 
    SimpleVector(const SimpleVector &other) {
        SimpleVector tmp;
        if (!other.IsEmpty()) {
            tmp.size_ = other.size_;
            tmp.capacity_ = other.capacity_;
            tmp.data = other.data;
        }
        swap(tmp);
    }
 
    SimpleVector &operator=(const SimpleVector &rhs) {
        if (this != &rhs) {
            auto rhs_copy(rhs);
            swap(rhs_copy);
        }
        return *this;
    }
 
    SimpleVector() noexcept = default;
 
    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) {
        if (size > 0) {
            size_ = size;
            capacity_ = CountCapacity(size);
            data = Iterator(capacity_);
            std::fill(data, data + size_, Type());
        }
    }
 
    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type &value) {
        if (size > 0) {
            size_ = size;
            capacity_ = CountCapacity(size);
            data = Iterator(capacity_);
            std::fill(data, data + size_, value);
        }
    }
 
    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) {
        size_ = init.size();
        capacity_ = CountCapacity(size_);
        data = Iterator(capacity_);
        std::copy(init.begin(), init.end(), data);
    }
 
    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }
 
    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }
 
    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }
 
    // Возвращает ссылку на элемент с индексом index
    Type &operator[](size_t index) noexcept {
        return data[index];
    }
 
    // Возвращает константную ссылку на элемент с индексом index
    const Type &operator[](size_t index) const noexcept {
        return data[index];
    }
 
    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type &At(size_t index) {
        if (index >= size_) throw std::out_of_range("");
        return data[index];
    }
 
    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type &At(size_t index) const {
        if (index >= size_) throw std::out_of_range("");
        return data[index];
    }
 
    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }
 
    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size <= size_) {
            size_ = new_size;
        } else if (new_size <= capacity_) {
            std::fill(data + size_, data + capacity_, Type());
        } else {
            //while (new_size > capacity_) capacity_ *= 2;
            capacity_ = new_size;
            auto tmp = Iterator(capacity_);
            std::fill(tmp, tmp + capacity_, Type());
            std::copy(data, data + size_, tmp);
            size_ = new_size;
            delete[] data;
            data = tmp;
        }
    }
 
    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type &item) {
        if (size_ < capacity_) {
            *(data + size_++) = item;
        } else {
            capacity_ *= 2;
            ArrayPtr<Type> tmp(capacity_);
            std::copy(data, data + size_, tmp.Get());
            tmp[size_++] = item;
            delete[] data;
            data = tmp.Release();
        }
    }
 
    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(Iterator pos, const Type &value) {
        if (size_ < capacity_) {
            std::copy_backward(Iterator(pos), data + size_++, pos+1); //TODO check res
            *pos = value;
        } else {
            capacity_ *= 2;
            ArrayPtr<Type> tmp(capacity_);
            std::copy(data, pos, tmp.Get());
            *pos = value; //todo
            std::copy(pos, data + size_++, tmp.Get());
            delete[] data;
            data = tmp.Release();
        }
        return Iterator(&value);
    }
 
    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if (!IsEmpty()) --size_;
    }
 
    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        std::copy_backward(Iterator(pos + 1), data + size_, Iterator(pos)); //TODO check res
        --size_;
        return Iterator(pos - 1);
    }
 
    // Обменивает значение с другим вектором
    void swap(SimpleVector &other) noexcept {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(data, other.data);
    }
 
    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return data;
    }
 
    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return data + size_;
    }
 
    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return data;
    }
 
    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return data + size_;
    }
 
    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return data;
    }
 
    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return data + size_;
    }
 
    size_t CountCapacity(size_t size) {
        //capacity_ = 1;
        //while (capacity_ < size) {
        //    capacity_ *= 2;
        //}
        //return capacity_;
        return size;
    }
 
private:
    Iterator data;
    size_t size_ = 0;
    size_t capacity_ = 0;
};
 
template<typename Type>
inline bool operator==(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
    return lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}
 
template<typename Type>
inline bool operator!=(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
    return !(operator==(lhs, rhs));
}
 
template<typename Type>
inline bool operator<(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                        rhs.begin(), rhs.end());
}
 
template<typename Type>
inline bool operator<=(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
    return (operator==(lhs, rhs)) || (operator<(lhs, rhs));
}
 
template<typename Type>
inline bool operator>(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
    return !(operator<=(lhs, rhs));
}
 
template<typename Type>
inline bool operator>=(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
    return (operator==(lhs, rhs)) || (operator>(lhs, rhs));
}

#pragma once

#include <cassert>
#include <initializer_list>
#include "array_ptr.h"
#include <algorithm>
#include<stdexcept>

using namespace std;

/*
В операторе = обработайте случай с самоприсваиванием.
Чтобы сделать оператор присваивания устойчивым к исключениям, примените идиому copy-and-swap.
При копировании элементов в пределах одного массива выберите между std::copy и std::copy_backward в зависимости от направления копирования элементов.
*/

template <typename Type>
class SimpleVector {
public:
	using Iterator = Type*;
	using ConstIterator = const Type*;

	//using Iterator = ArrayPtr<Type>;
	//using ConstIterator = const ArrayPtr<Type>;

	SimpleVector() noexcept = default;

    SimpleVector(const SimpleVector& other) {
        // Напишите тело конструктора самостоятельно
    }
    
    explicit SimpleVector(size_t size) :
		array_ptr_(size) {
		size_ = size;
		capacity_ = size;
        //std::fill(data, data + size_, Type());
        //array_ptr_.Get() + new_size
        std::fill(array_ptr_.Get(), array_ptr_.Get() + size_, Type());
        
        
	}

	// Создаёт вектор из size элементов, инициализированных значением value
	SimpleVector(size_t size, const Type& value) :
		array_ptr_(size) {
		size_ = size;
		capacity_ = size;
		for (size_t i = 0; i < size; i++) {
			array_ptr_[i] = value;
		}
	}

	// Создаёт вектор из std::initializer_list
	SimpleVector(std::initializer_list<Type> init) : array_ptr_(init.size()) {
		std::copy(init.begin(), init.end(), array_ptr_.Get());
		size_ = init.size();
		capacity_ = size_;

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
	Type& operator[](size_t index) noexcept {
		return array_ptr_[index];
	}

	// Возвращает константную ссылку на элемент с индексом index
	const Type& operator[](size_t index) const noexcept {
		return array_ptr_[index];
	}

	// Возвращает константную ссылку на элемент с индексом index
	// Выбрасывает исключение std::out_of_range, если index >= size
	Type& At(size_t index) {
		if (index >= size_) {
			throw std::out_of_range("index out of range");
			//throw out_of_range("index out of range");
		}
		return array_ptr_[index];
	}

	// Возвращает константную ссылку на элемент с индексом index
	// Выбрасывает исключение std::out_of_range, если index >= size
	const Type& At(size_t index) const {
		// Напишите тело самостоятельно
		if (index >= size_) {
			throw std::out_of_range("index out of range");
		}
		return array_ptr_[index];
	}

	// Обнуляет размер массива, не изменяя его вместимость
	void Clear() noexcept {
		// Напишите тело самостоятельно
		//Resiz e(0);
		size_ = 0;
	}
    
    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> tmp(new_capacity);
            std::move(begin(), end(), tmp.Get());
            array_ptr_.swap(tmp);
            capacity_ = new_capacity;
        }
    }
	// Изменяет размер массива.
	// При увеличении размера новые элементы получают значение по умолчанию для типа Type
		void Resize(size_t new_size) {
        if (new_size > size_) {
            if (new_size <= capacity_) {
                std::generate(end(), array_ptr_.Get() + new_size, [] { return Type(); });
            } else {
                //while (new_size > capacity_) capacity_ *= 2;
                Reserve(new_size);
                std::generate(end(), array_ptr_.Get() + new_size, [] { return Type(); });
            }
        }
        size_ = new_size;
    }

	// Возвращает итератор на начало массива
	// Для пустого массива может быть равен (или не равен) nullptr
	Iterator begin() noexcept {
		// Напишите тело самостоятельно
		//return array_ptr_.begin();
		return array_ptr_.Get();
	}

	// Возвращает итератор на элемент, следующий за последним
	// Для пустого массива может быть равен (или не равен) nullptr
	Iterator end() noexcept {
		// Напишите тело самостоятельно
		//return array_ptr_.end();
		return array_ptr_.Get() + size_;

	}

	// Возвращает константный итератор на начало массива
	// Для пустого массива может быть равен (или не равен) nullptr
	ConstIterator begin() const noexcept {
		// Напишите тело самостоятельно
		//return array_ptr_.cbegin();
		return array_ptr_.Get();
	}

	// Возвращает итератор на элемент, следующий за последним
	// Для пустого массива может быть равен (или не равен) nullptr
	ConstIterator end() const noexcept {
		// Напишите тело самостоятельно
		//return array_ptr_.cend();
		return array_ptr_.Get() + size_;
	}

	// Возвращает константный итератор на начало массива
	// Для пустого массива может быть равен (или не равен) nullptr
	ConstIterator cbegin() const noexcept {
		// Напишите тело самостоятельно
		//return array_ptr_.cbegin();
		return array_ptr_.Get();
	}

	// Возвращает итератор на элемент, следующий за последним
	// Для пустого массива может быть равен (или не равен) nullptr
	ConstIterator cend() const noexcept {
		// Напишите тело самостоятельно
		//return array_ptr_.cend();
		return array_ptr_.Get() + size_;
	}
    
    ////////////////////////////////////////////////////////////////////////////
    


    SimpleVector& operator=(const SimpleVector& rhs) {
        // Напишите тело конструктора самостоятельно
        return *this;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        // Напишите тело самостоятельно
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        // Напишите тело самостоятельно
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        // Напишите тело самостоятельно
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        // Напишите тело самостоятельно
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        // Напишите тело самостоятельно
    }
    

private:
	ArrayPtr<Type> array_ptr_;
	size_t size_ = 0;
	size_t capacity_ = 0;
	Iterator data;

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

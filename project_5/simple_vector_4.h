#pragma once

#include <cassert>
#include <initializer_list>
#include "array_ptr.h"
#include <algorithm>
#include<stdexcept>

using namespace std;

/*
В методе Resize отдельно обработайте три ситуации: новый размер меньше или равен текущему, новый размер не превышает его вместимости, новый размер превышает текущую вместимость вектора.
Если при изменении размера массива новый размер вектора превышает его текущую вместимость, создайте новый массив с нужной вместимостью, скопируйте в него прежнее содержимое и заполните остальные элементы значением по умолчанию. Затем старый массив можно удалить и использовать копию. После этого не забудьте обновить размер и вместимость вектора.
Если при увеличении размера массива новый размер вектора не превышает его вместимость, заполните добавленные элементы значением по умолчанию для типа Type.
При уменьшении размера вектора просто уменьшите его размер.
Примените алгоритмы std::copy и std::fill для копирования элементов массива и заполнения их некоторым значением.
В методе Clear достаточно обнулить размер массива.
*/

template <typename Type>
class SimpleVector {
public:
	using Iterator = Type*;
	using ConstIterator = const Type*;

	//using Iterator = ArrayPtr<Type>;
	//using ConstIterator = const ArrayPtr<Type>;

	SimpleVector() noexcept = default;

	// Создаёт вектор из size элементов, инициализированных значением по умолчанию
	explicit SimpleVector(size_t size) :
		array_ptr_(size) {
		size_ = size;
		capacity_ = size;
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
        
       /* void Resize(size_t new_size) {
            if (new_size <= size_) {
                size_ = new_size;
            } else if (new_size <= capacity_) {
                std::fill(array_ptr_.Get() + size_, array_ptr_.Get() + capacity_, Type());
            } else {
                size_t new_capacity = max(new_size, capacity_ * 2);
                ArrayPtr<Type> new_array_ptr_(new_capacity);
                std::copy(array_ptr_.Get(), array_ptr_.Get() + size_, new_array_ptr_.Get());
                array_ptr_.swap(new_array_ptr_);
                auto tmp = array_ptr_.Get() + size_;
                std::fill(tmp, tmp + (new_capacity- size_), Type());
                capacity_ = new_capacity; 
                size_ = new_size;
            }
        }
        */


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

private:
	ArrayPtr<Type> array_ptr_;
	size_t size_ = 0;
	size_t capacity_ = 0;
	Iterator data;

};

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
    
    // Пробую напрямую
    /*SimpleVector(const SimpleVector &other) :
            array_ptr_(other.array_ptr_), size_(other.size_), capacity_(other.capacity_) {
        // Напишите тело конструктора самостоятельно
        std::copy(other.begin(), other.end(), begin());


    }*/

	SimpleVector(const SimpleVector& other) {
		// Напишите тело конструктора самостоятельно
		SimpleVector tmp;
		if (!other.IsEmpty()) {
			tmp.size_ = other.size_;
			tmp.capacity_ = other.capacity_;
		}
		swap(tmp);

	}

	explicit SimpleVector(size_t size) :
		array_ptr_(size) {
		size_ = size;
		capacity_ = size;
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
			}
			else {
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
    
    
    SimpleVector& operator=(const SimpleVector &rhs) {
        // Напишите тело конструктора самостоятельно
        delete[] array_ptr_.Release();
        /*pointer*/ ArrayPtr<Type> newptr(rhs.size_);
        std::copy(rhs.begin(), rhs.end(), newptr.Get());
        array_ptr_.swap(newptr);
        size_ = rhs.size_;
        capacity_ = size_;
        return *this;
    }
    
    // Старая версия 
    /*
	SimpleVector& operator=(const SimpleVector& rhs) {
		// Напишите тело конструктора самостоятельно
		if (this != &rhs) {   
        
			SimpleVector tmp(rhs);  // инициализируб копию вектором 
			swap(tmp);
		}
		return *this;
	}*/

	// Добавляет элемент в конец вектора
	// При нехватке места увеличивает вдвое вместимость вектора
	void PushBack(const Type& item) {
		// Напишите тело самостоятельно
		if (size_ < capacity_) { 
        array_ptr_[size_] = item; 
        ++size_; 
        }
		else {
            ArrayPtr<Type> tmp(capacity_ * 2); //выделите новый массив с удвоенной вместимостью
			std::copy(tmp.Get(), tmp.Get() + size_, array_ptr_.Get());  //скопируйте в него элементы исходного массива
			tmp[size_] = item; // а в конец поместите вставляемый элемент
            
			//После этого можно обновить размер и вместимость вектора, переключиться на новый массив, а старый массив — удалить.*/
			array_ptr_.swap(tmp); //swap(tmp);
			size_ = size_ + 1;
			capacity_ = capacity_ * 2;
		}
	}

	// Вставляет значение value в позицию pos.
	// Возвращает итератор на вставленное значение
	// Если перед вставкой значения вектор был заполнен полностью,
	// вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1

    Iterator Insert(ConstIterator pos, const Type& value) {
		// Напишите тело самостоятельно
		if (size_ > 0 && size_ < capacity_) {
            ++size_; // увеличиваю размер чтобы было куда сдвинуть вправо 
            std::copy_backward(Iterator(pos), end(), end() + 1);
            array_ptr_[pos-array_ptr_.Get()] = value;
		}
		else {
			int tmp_capacity = 0;
			if (capacity_ == 0) { tmp_capacity = 1; }
			else { tmp_capacity = capacity_ * 2; }
			ArrayPtr<Type> tmp(tmp_capacity);
            std::copy(begin(), Iterator(pos), tmp.Get()); // копируются элементы, которые предшествуют вставляемому
            
			tmp[pos - tmp.Get()] = value;  //сам вставляемый элемент
            std::copy( Iterator(pos), end(),  tmp.Get() ); // и элементы, следующие за ним
            
			//В конце вектор обновляет свой размер и вместимость, начинает ссылаться на новый массив, а старый массив удаляет:
			//swap(tmp);
            //array_ptr_ = tmp;  - так не работает
            array_ptr_.swap(tmp);
			size_ = size_ + 1;
			capacity_ = tmp_capacity;

		}
        return Iterator(&value);
	}

	// "Удаляет" последний элемент вектора. Вектор не должен быть пустым
	void PopBack() noexcept {
		// Напишите тело самостоятельно
		if (!IsEmpty()) --size_;
	}

	// Удаляет элемент вектора в указанной позиции
	Iterator Erase(ConstIterator pos) {
		// Напишите тело самостоятельно
        std::copy_backward(Iterator(pos+1), end(), Iterator(pos));
		--size_;
		return Iterator(pos); // следующий за удалернным будет именно pos как я понял 

	}

	// Обменивает значение с другим вектором
	void swap(SimpleVector& other) noexcept {
		// Напишите тело самостоятельно
		size_ = other.size_;
		capacity_ = other.size_;
	}


private:
	ArrayPtr<Type> array_ptr_;
	size_t size_ = 0;
	size_t capacity_ = 0;


};



template<typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return !(operator==(lhs, rhs));
}

template<typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(),
		rhs.begin(), rhs.end());
}

template<typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return (operator==(lhs, rhs)) || (operator<(lhs, rhs));
}

template<typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return !(operator<=(lhs, rhs));
}

template<typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return (operator==(lhs, rhs)) || (operator>(lhs, rhs));
}

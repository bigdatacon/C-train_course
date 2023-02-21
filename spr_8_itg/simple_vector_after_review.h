#pragma once

#include <cassert>
#include <initializer_list>
#include "array_ptr.h"
#include <algorithm>
#include<stdexcept>

using namespace std;


class ReserveProxyObj {
public:
	ReserveProxyObj(size_t i) {
		capacity_to_reserve = i;
	}
	size_t capacity_to_reserve;
};

template <typename Type>
class SimpleVector {
public:
	using Iterator = Type*;
	using ConstIterator = const Type*;
	SimpleVector() noexcept = default;


	SimpleVector(ReserveProxyObj other) {
		// Напишите тело конструктора самостоятельно
		Reserve(other.capacity_to_reserve);
	}

	SimpleVector(const SimpleVector& other) {
		// Напишите тело конструктора самостоятельно
		if (!other.IsEmpty()) {
			SimpleVector tmp(other.size_);
			std::copy(other.begin(), other.end(), tmp.array_ptr_.Get());
			swap(tmp);
		}
	}
	// Конструктор для rvalue
	SimpleVector(SimpleVector&& other) {
		// Напишите тело конструктора самостоятельно
		swap(other);
	}


	explicit SimpleVector(size_t size) :
		array_ptr_(size) {
		size_ = size;
		capacity_ = size;
		std::generate(array_ptr_.Get(), array_ptr_.Get() + size_, [] { return Type(); });
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
		assert(index < size_);
		return array_ptr_[index];
	}

	// Возвращает константную ссылку на элемент с индексом index
	const Type& operator[](size_t index) const noexcept {
		assert(index < size_);
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


	// для rvalue
	void Resize(size_t new_size) {
		if (new_size > size_) {
			if (new_size <= capacity_) {
				//std::generate(end(), array_ptr_.Get() + new_size, Type{} );
				std::generate(end(), array_ptr_.Get() + new_size, [] { return Type(); });
			}
			else {
				//while (new_size > capacity_) capacity_ *= 2;
				Reserve(new_size);
				//std::generate(end(), array_ptr_.Get() + new_size, Type{} );
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



	SimpleVector& operator=(const SimpleVector& rhs) {
		// Напишите тело конструктора самостоятельно
		delete[] array_ptr_.Release();
		/*pointer*/ ArrayPtr<Type> newptr(rhs.size_);
		std::copy(rhs.begin(), rhs.end(), newptr.Get());
		array_ptr_.swap(newptr);
		size_ = rhs.size_;
		capacity_ = size_;
		return *this;
	}

	//Присваивание для rvalue 
	SimpleVector& operator=(SimpleVector&& rhs) {
		// Напишите тело конструктора самостоятельно
		delete[] array_ptr_.Release();
		/*pointer*/ ArrayPtr<Type> newptr(rhs.size_);
		std::move(rhs.begin(), rhs.end(), newptr.Get());
		array_ptr_.swap(newptr);
		size_ = std::move(rhs.size_);
		capacity_ = size_;
		return *this;
	}


	// Добавляет элемент в конец вектора
	// При нехватке места увеличивает вдвое вместимость вектора

	void PushBack(const Type& item) {
		// Напишите тело самостоятельно
		if (IsEmpty()) {
			if (!capacity_) {
				Reserve(10);
			}
			array_ptr_[0] = item;
			size_ = 1;
			return;
		}
		if (size_ < capacity_) {
			array_ptr_[size_] = item;
			++size_;
		}
		else {
			ArrayPtr<Type> tmp(capacity_ * 2); //выделите новый массив с удвоенной вместимостью
			//std::copy(tmp.Get(), tmp.Get() + size_, array_ptr_.Get()); // наоборот было изначально
			std::copy(array_ptr_.Get(), array_ptr_.Get() + size_, tmp.Get());   //скопируйте в него элементы исходного массива
			tmp[size_] = item; // а в конец поместите вставляемый элемент
			array_ptr_.swap(tmp);
			size_ = size_ + 1;
			capacity_ = capacity_ * 2;
		}
	}

	// Push_back для rvalues ссылки 
	void PushBack(Type&& item) {
		// Напишите тело самостоятельно
		if (IsEmpty()) {
			if (!capacity_) {
				Reserve(10);
			}
			array_ptr_[0] = std::move(item);
			size_ = 1;
			return;
		}
		if (size_ < capacity_) {
			array_ptr_[size_] = std::move(item);
			++size_;
		}
		else {
			ArrayPtr<Type> tmp(capacity_ * 2); //выделите новый массив с удвоенной вместимостью
			//std::copy(tmp.Get(), tmp.Get() + size_, array_ptr_.Get()); // наоборот было изначально
			std::move(array_ptr_.Get(), array_ptr_.Get() + size_, tmp.Get());   //скопируйте в него элементы исходного массива
			tmp[size_] = std::move(item); // а в конец поместите вставляемый элемент
			array_ptr_.swap(tmp);
			size_ = size_ + 1;
			capacity_ = capacity_ * 2;
		}
	}


	// Вставляет значение value в позицию pos.
	// Возвращает итератор на вставленное значение
	// Если перед вставкой значения вектор был заполнен полностью,
	// вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1


	Iterator Insert(ConstIterator pos, const Type& value) {
		assert(pos >= begin() && pos <= end());
		if (pos == end()) {
			PushBack(value);
			return end() - 1;
		}
		if (size_ < capacity_) {
			std::copy_backward((Iterator)pos, end(), end() + 1);
			*((Iterator)pos) = value;
			++size_;
			return (Iterator)pos;
		}


		SimpleVector<Type> swap_ptr((2 * capacity_));
		std::copy(begin(), (Iterator)pos, swap_ptr.begin());
		std::copy((Iterator)pos, end(), swap_ptr.begin() + ((Iterator)pos - begin() + 1));
		auto return_it = swap_ptr.begin() + (pos - begin());
		*return_it = value;
		capacity_ = 2 * capacity_;
		++size_;
		array_ptr_.swap(swap_ptr.array_ptr_);
		return return_it;
	}

	// для rvalue ссылки 
	Iterator Insert(ConstIterator pos, Type&& value) {
		assert(pos >= begin() && pos <= end());
		if (pos == end()) {
			PushBack(std::move(value));
			return end() - 1;
		}
		if (size_ < capacity_) {
			for (auto p = end(); p != (Iterator)pos; p = prev(p)) {
				*p = std::move(*prev(p));
			}
			*((Iterator)pos) = std::move(value);
			++size_;
			return (Iterator)pos;
		}


		SimpleVector<Type> swap_ptr((2 * capacity_));
		std::move(begin(), (Iterator)pos, swap_ptr.begin());
		std::move((Iterator)pos, end(), swap_ptr.begin() + ((Iterator)pos - begin() + 1));
		auto return_it = swap_ptr.begin() + (pos - begin());
		*return_it = std::move(value);
		capacity_ = 2 * capacity_;
		++size_;
		array_ptr_.swap(swap_ptr.array_ptr_);
		return return_it;
	}


	// "Удаляет" последний элемент вектора. Вектор не должен быть пустым
	void PopBack() noexcept {
		// Напишите тело самостоятельно
		if (!IsEmpty()) --size_;
	}



	Iterator Erase(ConstIterator pos) {
		// Напишите тело самостоятельно
		assert(pos >= begin() && pos < end());
		std::move(std::next(Iterator(pos)), end(), Iterator(pos)); // сдвигаю все элементы следующие за pos на 1 влево 
		--size_;
		return Iterator(pos);
	}


	// Обменивает значение с другим вектором
	void swap(SimpleVector& other) noexcept {
		// Напишите тело самостоятельно
		array_ptr_.swap(other.array_ptr_);
		std::swap(size_, other.size_);
		std::swap(capacity_, other.capacity_);

	}

	// для rvalue ссылки 
	void swap(SimpleVector&& other) noexcept {
		// Напишите тело самостоятельно
		array_ptr_.swap(other.array_ptr_);
		std::swap(size_, other.size_);
		std::swap(capacity_, other.capacity_);
	}


private:
	ArrayPtr<Type> array_ptr_;
	size_t size_ = 0;
	size_t capacity_ = 0;


};

//огда функция (не метод!) будет иметь следующую сигнатуру:
ReserveProxyObj Reserve(size_t capacity_to_reserve) {
	return ReserveProxyObj(capacity_to_reserve);
}

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

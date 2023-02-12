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
	SimpleVector(std::initializer_list<Type> init) :  array_ptr_(init.size()) {
        std::copy (init.begin(), init.end(), array_ptr_.Get());
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
		Resize(0);
	}

	// Изменяет размер массива.
	// При увеличении размера новые элементы получают значение по умолчанию для типа Type
	void Resize(size_t new_size) {
		// Напишите тело самостоятельно
		if (new_size <= size_) { size_ = new_size; }

		// случай когда увеличивается размер вектора до размера <= capacity
		if (new_size > size_ && new_size <= capacity_) {
			Type& last_el = array_ptr_[-1]; // получаю послдений элемент вектора 
			auto last_it = std::find(array_ptr_.begin(), array_ptr_.end(), last_el); // нахожу итератор на последний элемента вектора 

			size_t add_new_el = new_size - size_;  // количество элементов сколько нужно добавить 
			Type el; // создаю переменную со значением типа T
			for (size_t i = 1; i < add_new_el; ++i) {

				std::fill(last_it, ++last_it, el);   // заполняю вектор значениями типа T
				++last_it; // увеличиваю итератор на 1 шаг вперед 
			}
		}
		// случай когда увеличивается размер вектора до размера > capacity
		/*
		Самое интересное происходит, когда новый размер превышает текущую вместимость SimpleVector. В этом случае SimpleVector создаёт новый массив большего размера в динамической памяти, куда копирует элементы исходного массива и инициализирует остальные элементы значением по умолчанию. Для копирования элементов подходит алгоритм std::copy.
Подобно std::vector во многих реализациях стандартной библиотеки, новую вместимость SimpleVector можно выбрать как максимум из new_capacity и capacity_ * 2. Удваивание вместимости минимизирует частоту копирований элементов из одного массива в другой.
После копирования и заполнения элементов нулевым значением можно обновить size_ и capacity_, а старый массив — удалить. Так вы обеспечите строгую гарантию безопасности исключений. Умный указатель ArrayPtr позволит сделать код не только надёжнее, но и проще.
		*/
		if (new_size > capacity_) {
			//0. определяю новую вместимость 
			//Подобно std::vector во многих реализациях стандартной библиотеки, новую вместимость SimpleVector можно выбрать как максимум из new_capacity и capacity_
			new_size = max(new_size, capacity_ * 2);

				ArrayPtr<Type> new_array_ptr_(new_size); // 1. создаю новый массив 
			array_ptr_.swap(new_array_ptr_); // обмениваю новый и старый массив 
			std::copy(new_array_ptr_.begin(), new_array_ptr_.end(), array_ptr_.begin());  // копирую в него элементы из прежнего массива 
			//инициализирует остальные элементы значением по умолчанию
			Type el; // создаю переменную со значением типа T
			size_t add_new_el = new_size - size_;
			Type& last_el = array_ptr_[-1]; // получаю послдений элемент вектора 
			auto last_it = std::find(array_ptr_.begin(), array_ptr_.end(), last_el); // нахожу итератор на последний элемента вектора 
			//заполняю остальные элементы значениями пол умолчанию 
			for (size_t i = 1; i < add_new_el; ++i) {
				fill(last_it, array_ptr_.end(), el);  // заполняю вектор значениями типа T
			}

			//После копирования и заполнения элементов нулевым значением можно обновить size_ и capacity_, а старый массив — удалить. Так вы обеспечите строгую гарантию безопасности исключений. Умный указатель ArrayPtr позволит сделать код не только надёжнее, но и проще.

			capacity_ = new_size; //емкость становится как величина на которую делал resize, при этом size_ не меняю, ведь вектор при resize заполнялся 
			//элементами из старого вектора. Не знаю как удалить старый вектор 


		}
	}

			// Возвращает итератор на начало массива
			// Для пустого массива может быть равен (или не равен) nullptr
			Iterator begin() noexcept {
				// Напишите тело самостоятельно
				return array_ptr_.begin();
			}

			// Возвращает итератор на элемент, следующий за последним
			// Для пустого массива может быть равен (или не равен) nullptr
			Iterator end() noexcept {
				// Напишите тело самостоятельно
				return array_ptr_.end();
			}

			// Возвращает константный итератор на начало массива
			// Для пустого массива может быть равен (или не равен) nullptr
			ConstIterator begin() const noexcept {
				// Напишите тело самостоятельно
				return array_ptr_.cbegin();
			}

			// Возвращает итератор на элемент, следующий за последним
			// Для пустого массива может быть равен (или не равен) nullptr
			ConstIterator end() const noexcept {
				// Напишите тело самостоятельно
				return array_ptr_.cend();
			}

			// Возвращает константный итератор на начало массива
			// Для пустого массива может быть равен (или не равен) nullptr
			ConstIterator cbegin() const noexcept {
				// Напишите тело самостоятельно
				return array_ptr_.cbegin();
			}

			// Возвращает итератор на элемент, следующий за последним
			// Для пустого массива может быть равен (или не равен) nullptr
			ConstIterator cend() const noexcept {
				// Напишите тело самостоятельно
				return array_ptr_.cend();
			}

private:
	ArrayPtr<Type> array_ptr_;
	size_t size_ = 0;
	size_t capacity_ = 0;

	};

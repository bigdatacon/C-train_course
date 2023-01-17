#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <set>


using namespace std;
/*
Задание 3
Продолжите исследовать стек и то, как его можно реализовать.
В этот раз напишите класс SortedStack, который бы всегда поддерживал свои элементы в отсортированном виде. При вставке сортировка должна сохраняться. На вершине стека всегда должен быть минимум. Не используйте в этой задаче алгоритмы сортировки — её можно решить без них.
Методы стека перестанут быть такими простыми, эффективными и быстрыми, как раньше. В них могут появиться условия и циклы. Это цена, которую вы платите за то, что стек будет постоянно находиться в сортированном состоянии.
Используйте заготовку и допишите необходимую реализацию методов Push и Pop:

Подсказка
Вам нужно вставить новый элемент. Ответьте на некие три вопроса, которые вы где-то уже видели.
Какие параметры принимает функция? Функция Push принимает элемент для вставки. Это вы уже знаете.
В каком случае мы можем остановить алгоритм поиска места для вставки и вставить элемент без дальнейших действий? Если стек пуст или если элемент больше или равен элементу на вершине стека.
Что делать, если алгоритм не остановился? Это значит, что элемент сверху больше того, который вы хотите вставить. Вытащите его, но сохраните, чтобы потом вернуть. Теперь у вас есть стек без верхнего элемента. Попробуйте повторить для него весь алгоритм.
В итоге вы найдёте место для вставки, вставите новый элемент, а дальше нужно будет вернуть все вытащенные элементы на место.
Вывод:
Вставляемый элемент = 5
5
Вставляемый элемент = 2
5 2
Вставляемый элемент = 4
5 4 2
Вставляемый элемент = 3
5 4 3 2
Вставляемый элемент = 1
5 4 3 2 1 
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
		//return elements_;
		return elements_.back();
		// напишите реализацию
	}
	Type& Peek() {
		// напишите реализацию
		//return elements_;
		return elements_.back();
	}
	void Print() const {
		// напишите реализацию
		for (auto el : elements_) { cout << el << " "; }
		cout << endl;
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

template <typename Type>
class StackMin {
public:
	void Push(const Type& element) {
		// напишите реализацию метода
		//elements_.push_back(element);
		 // для первого добавления минимальный элемент это 1 элемент списка 
		if (elements_.IsEmpty()) {
			min_element_ = element;
			elements_.Push(element);
			elements_minimus_.push_back({element, min_element_});

		}
		else {
			bool new_min = element < min_element_;
			if (new_min) {
                min_element_ = element;
                elements_.Push(element);
                elements_minimus_.push_back({element, min_element_});
			}
			else { 
            elements_.Push(element);
            elements_minimus_.push_back({element, min_element_}); }

		}
	}
		void Pop() {
			// напишите реализацию метода
			//elements_.pop_back();
			elements_.Pop();
            elements_minimus_.pop_back();
            
		}
		const Type& Peek() const {
			return elements_.Peek();
		}
		Type& Peek() {
			return elements_.Peek();
		}
		void Print() const {
			// работу этого метода мы проверять не будем,
			// но если он вам нужен, то можете его реализовать
			elements_.Print();
		}
        
        void Print_mins() const {
			// работу этого метода мы проверять не будем,
			// но если он вам нужен, то можете его реализовать
			for (auto el : elements_minimus_) {cout << "first: " << el.first << " second :" << el.second << " ";}
		}
        
		uint64_t Size() const {
			return elements_.size();
		}
		bool IsEmpty() const {
			return elements_.IsEmpty();
		}
		const Type& PeekMin() const {
			// напишите реализацию метода
			return elements_minimus_.back().second;
		}
		Type& PeekMin() {
			// напишите реализацию метода
			return elements_minimus_.back().second;
		}
private:
	Stack<Type> elements_;
	// возможно, здесь вам понадобится что-то изменить
	Type min_element_;  // задаю начальный максимум нулем
    //vector<map<Type, Type>> elements_minimus_;
    vector<pair<Type, Type>> elements_minimus_;

	};


template <typename Type>
class SortedStack {
public:
    void Push(const Type& element) {
    // напишите реализацию метода
    	if (elements_.IsEmpty()) {
			elements_.Push(element);

		}
		else {
            // Запрашиваем итератор на первый элемент, не меньший border;
            // если такого элемента нет, то мы получим numbers.end()
            //auto first_not_less = elements_.lower_bound(element);  -- ни так ни так не работает 
            auto first_not_less = elements_.lower_bound(elements_.begin(), elements_.end(), element);
            elements_.insert(first_not_less, element);
    }
    }
    void Pop() {
    // напишите реализацию метода
        elements_.Pop();
    }
    const Type& Peek() const {
    return elements_.Peek();
    }
    Type& Peek() {
    return elements_.Peek();
    }
    void Print() const {
    elements_.Print();
    }
    uint64_t Size() const {
    return elements_.Size();
    }
    bool IsEmpty() const {
    return elements_.IsEmpty();
    }
private:
    Stack<Type> elements_;
};



int main() {
    SortedStack<int> stack;
    vector<int> values(5);
    // заполняем вектор для тестирования нашего стека
    iota(values.begin(), values.end(), 1);
    // перемешиваем значения
    random_shuffle(values.begin(), values.end());
    // заполняем стек и проверяем, что сортировка сохраняется после каждой вставки
    for (int i = 0; i < 5; ++i) {
        cout << "Вставляемый элемент = "s << values[i] << endl;
        stack.Push(values[i]);
        stack.Print();
    }
}

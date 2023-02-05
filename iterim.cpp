#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <iterator>

/*
Задание
Разработайте поддержку перебора элементов контейнера SingleLinkedList.
Реализуйте шаблонный класс BasicIterator, на основе которого будут объявлены константный и неконстантный итераторы списка.
В классе списка реализуйте константную и неконстантную версии методов begin и end, которые возвращают итераторы на первый элемент контейнера и позицию, следующую за последним элементом.
Чтобы получать константные итераторы было удобно, реализуйте методы cbegin и cend.

Ограничения
Инкремент и разыменование итератора, ссылающегося на позицию за последним элементом списка, — недопустимые операции. Очистка или разрушение списка делают невалидными его итераторы.
Любые операции над невалидным итератором, кроме естественного вызова деструктора, тоже недопустимы.
Гарантируется, что тренажёр не будет совершать таких операций. Не усложняйте класс списка и итератора, чтобы выявить ситуации некорректного использования итераторов.

Подсказка
Не переживайте, что end-итератор, который указывает на позицию, следующую за последним элементом, будет ссылаться на nullptr. Это корректная операция,
ведь разыменование и инкремент итератора не разрешены.
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


	template <typename ValueType>
	class BasicIterator {
		// Класс списка объявляется дружественным, чтобы из методов списка
		// был доступ к приватной области итератора
		friend class SingleLinkedList;

		// Конвертирующий конструктор итератора из указателя на узел списка
		explicit BasicIterator(Node* node) node_(node) {
			//assert(false);
			// Реализуйте конструктор самостоятельно
            //return node_ = node;  -- или так 
		}

	public:
		// Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора
		// Категория итератора — forward iterator
		// (итератор, который поддерживает операции инкремента и многократное разыменование)
		using iterator_category = std::forward_iterator_tag;
		// Тип элементов, по которым перемещается итератор
		using value_type = Type;
		// Тип, используемый для хранения смещения между итераторами
		using difference_type = std::ptrdiff_t;
		// Тип указателя на итерируемое значение
		using pointer = ValueType*;
		// Тип ссылки на итерируемое значение
		using reference = ValueType&;

		BasicIterator() = default;

		// Конвертирующий конструктор/конструктор копирования
		// При ValueType, совпадающем с Type, играет роль копирующего конструктора
		// При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
		BasicIterator(const BasicIterator<Type>& other) noexcept {
			//assert(false);
			// Реализуйте конструктор самостоятельно
            //node_ = node; - можно так  
            BasicIteratornode); //-- раз valu_type  other = Type - то просто копирую, вызываю копирующий конструктор 
		}

		// Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
		// пользовательского конструктора копирования, явно объявим оператор = и
		// попросим компилятор сгенерировать его за нас
		BasicIterator& operator=(const BasicIterator& rhs) = default;

		// Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
		// Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
		[[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
			//assert(false);
			// Заглушка. Реализуйте оператор самостоятельно
            return ( (this->node_ == other.node_) || (node_==nullptr && rhs==nullptr)  ) ; // нашел в интернете что если итератор ссылается на end то равен nullptr
		}

		// Оператор проверки итераторов на неравенство
		// Противоположен !=
		[[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
			//assert(false);
			// Заглушка. Реализуйте оператор самостоятельно
            return ( (this->node_ != other.node_) || (node_==nullptr && rhs!=nullptr) || (node_!=nullptr && rhs==nullptr)) ; // нашел в интернете что если итератор ссылается на end то равен nullptr
		}

		// Оператор сравнения итераторов (в роли второго аргумента итератор)
		// Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
		[[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
			//assert(false);
			// Заглушка. Реализуйте оператор самостоятельно
            return ( (this->node_ == other.node_) || (node_==nullptr && rhs==nullptr)  ) ; // нашел в интернете что если итератор ссылается на end то раве
		}

		// Оператор проверки итераторов на неравенство
		// Противоположен !=
		[[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
			//assert(false);
			// Заглушка. Реализуйте оператор самостоятельно
            return ( (this->node_ != other.node_) || (node_==nullptr && rhs!=nullptr) || (node_!=nullptr && rhs==nullptr)) ; // нашел в интернете что если итератор ссылает
		}

		// Оператор прединкремента. После его вызова итератор указывает на следующий элемент списка
		// Возвращает ссылку на самого себя
		// Инкремент итератора, не указывающего на существующий элемент списка, приводит к неопределённому поведению
		BasicIterator& operator++() noexcept {
			//assert(false);
			// Заглушка. Реализуйте оператор самостоятельно
            return *this;;  // взял из теории 
		}

		// Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка
		// Возвращает прежнее значение итератора
		// Инкремент итератора, не указывающего на существующий элемент списка,
		// приводит к неопределённому поведению
		BasicIterator operator++(int) noexcept {
			//assert(false);
			// Заглушка. Реализуйте оператор самостоятельно
            auto old_value(*this); // Сохраняем прежнее значение объекта для последующего возврата
            ++(*this); // используем логику префиксной формы инкремента
            return old_value;   // взято из теории 
		}

		// Операция разыменования. Возвращает ссылку на текущий элемент
		// Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
		// приводит к неопределённому поведению
		[[nodiscard]] reference operator*() const noexcept {
			//assert(false);
			// Не реализовано
			// Заглушка. Реализуйте оператор самостоятельно
            if (node_!= nullptr){
            return *node_; }
            else {return nullptr;}
		}

		// Операция доступа к члену класса. Возвращает указатель на текущий элемент списка
		// Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
		// приводит к неопределённому поведению
		[[nodiscard]] pointer operator->() const noexcept {
			assert(false);
			// Заглушка. Реализуйте оператор самостоятельно
            if (node_!= nullptr){
            return node_; }
            else {return nullptr;}   // тут также возвращаю nullptr, хотя видимо для пустого указателя нужно что то другое вернуть
		}

	private:
		Node* node_ = nullptr;
	};

public:
	// создаю конструктор по умолчанию 
	//SingleLinkedList() {} - так не работа
	Node* first;
	Node* last;
	SingleLinkedList() : first(nullptr), last(nullptr) {}

	~SingleLinkedList() {
		Clear();
	}
	using value_type = Type;
	using reference = value_type&;
	using const_reference = const value_type&;

	// Итератор, допускающий изменение элементов списка
	using Iterator = BasicIterator<Type>;
	// Константный итератор, предоставляющий доступ для чтения к элементам списка
	using ConstIterator = BasicIterator<const Type>;

	// Возвращает итератор, ссылающийся на первый элемент
	// Если список пустой, возвращённый итератор будет равен end()
	[[nodiscard]] Iterator begin() noexcept {
		assert(false);
		// Реализуйте самостоятельно
		return {};
	}

	// Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
	// Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] Iterator end() noexcept {
		assert(false);
		// Реализуйте самостоятельно
		return {};
	}

	// Возвращает константный итератор, ссылающийся на первый элемент
	// Если список пустой, возвращённый итератор будет равен end()
	// Результат вызова эквивалентен вызову метода cbegin()
	[[nodiscard]] ConstIterator begin() const noexcept {
		assert(false);
		// Реализуйте самостоятельно
		return {};
	}

	// Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
	// Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
	// Результат вызова эквивалентен вызову метода cend()
	[[nodiscard]] ConstIterator end() const noexcept {
		assert(false);
		// Реализуйте самостоятельно
		return {};
	}

	// Возвращает константный итератор, ссылающийся на первый элемент
	// Если список пустой, возвращённый итератор будет равен cend()
	[[nodiscard]] ConstIterator cbegin() const noexcept {
		assert(false);
		// Реализуйте самостоятельно
		return {};
	}

	// Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
	// Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] ConstIterator cend() const noexcept {
		assert(false);
		// Реализуйте самостоятельно
		return {};
	}


	// Возвращает количество элементов в списке за время O(1)
	[[nodiscard]] size_t GetSize() const noexcept {
		int size = 0;
		Node* p = first;
		while (p) {
			size += 1;
			p = p->next_node;
		}
		return size;

	}

	// Сообщает, пустой ли список за время O(1)
	[[nodiscard]] bool IsEmpty() const noexcept {
		// Заглушка. Реализуйте метод самостоятельно
		return first == nullptr;  // так нашел в интернете 
	}

	// Вставляет элемент value в начало списка за время O(1)
	void PushFront(const Type& value) {
		// Реализуйте метод самостоятельно
		first = new Node(value, first);
		++size_;
	}

	// Очищает список за время O(N)
	void Clear() noexcept {
		while (first) {
			Node* next = first->next_node;
			delete first;
			first = next;
		}
		first = nullptr; last = nullptr;
	}

private:
	// Фиктивный узел, используется для вставки "перед первым элементом"
	Node head_;
	size_t size_;
};



// Эта функция тестирует работу SingleLinkedList
void Test2() {
	// Итерирование по пустому списку
	{
		SingleLinkedList<int> list;
		// Константная ссылка для доступа к константным версиям begin()/end()
		const auto& const_list = list;

		// Итераторы begin и end у пустого диапазона равны друг другу
		assert(list.begin() == list.end());
		assert(const_list.begin() == const_list.end());
		assert(list.cbegin() == list.cend());
		assert(list.cbegin() == const_list.begin());
		assert(list.cend() == const_list.end());
	}

	// Итерирование по непустому списку
	{
		SingleLinkedList<int> list;
		const auto& const_list = list;

		list.PushFront(1);
		assert(list.GetSize() == 1u);
		assert(!list.IsEmpty());

		assert(const_list.begin() != const_list.end());
		assert(const_list.cbegin() != const_list.cend());
		assert(list.begin() != list.end());

		assert(const_list.begin() == const_list.cbegin());

		assert(*list.cbegin() == 1);
		*list.begin() = -1;
		assert(*list.cbegin() == -1);

		const auto old_begin = list.cbegin();
		list.PushFront(2);
		assert(list.GetSize() == 2);

		const auto new_begin = list.cbegin();
		assert(new_begin != old_begin);
		// Проверка прединкремента
		{
			auto new_begin_copy(new_begin);
			assert((++(new_begin_copy)) == old_begin);
		}
		// Проверка постинкремента
		{
			auto new_begin_copy(new_begin);
			assert(((new_begin_copy)++) == new_begin);
			assert(new_begin_copy == old_begin);
		}
		// Итератор, указывающий на позицию после последнего элемента, равен итератору end()
		{
			auto old_begin_copy(old_begin);
			assert((++old_begin_copy) == list.end());
		}
	}
	// Преобразование итераторов
	{
		SingleLinkedList<int> list;
		list.PushFront(1);
		// Конструирование ConstIterator из Iterator
		SingleLinkedList<int>::ConstIterator const_it(list.begin());
		assert(const_it == list.cbegin());
		assert(*const_it == *list.cbegin());

		SingleLinkedList<int>::ConstIterator const_it1;
		// Присваивание ConstIterator'у значения Iterator
		const_it1 = list.begin();
		assert(const_it1 == const_it);
	}
	// Проверка оператора ->
	{
		using namespace std;
		SingleLinkedList<std::string> string_list;

		string_list.PushFront("one"s);
		assert(string_list.cbegin()->length() == 3u);
		string_list.begin()->push_back('!');
		assert(*string_list.begin() == "one!"s);
	}
}

int main() {
	Test2();
}



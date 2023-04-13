#pragma once

#include <string>

using namespace std;


class Person;

// Наблюдатель за состоянием человека.
class PersonObserver {
public:
	virtual void OnSatisfactionChanged(Person& person, int old_value, int new_value) {
	}
protected:
	// Класс PersonObserver не предназначен для удаления напрямую
	~PersonObserver() = default;
};

/*
	Человек.
	При изменении уровня удовлетворённости уведомляет
	связанного с ним наблюдателя
*/
class Person {
public:
	Person(const string& name, int age)
		: name_(name)
		, age_(age) {
	}

	int GetSatisfaction() const {
		return satisfaction_;
	}

	const std::string& GetName() const {
		return name_;
	}
	void SetObserver(PersonObserver* observer) {
		observer_ = observer;
	}

	int GetAge() const {
		return age_;
	}

	// Увеличивает на 1 количество походов на танцы
	// Увеличивает удовлетворённость на 1
	virtual void Dance() {
		SetSatisfaction(satisfaction_ + 1);
		dance_count_ += 1;
	}

	int GetDanceCount() const {
		return dance_count_;
	}

	// Прожить день. Реализация в базовом классе ничего не делает
	virtual void LiveADay() {
		// Подклассы могут переопределить этот метод
	}
	virtual ~Person() = default;

protected:
	void SetSatisfaction(int value) {
		if (satisfaction_ != value) {
			int old_satisfaction = satisfaction_;
			satisfaction_ = value;
			// Если у человека есть наблюдатель, сообщаем ему об изменении удовлетворённости
			if (observer_) {
				observer_->OnSatisfactionChanged(*this, old_satisfaction, satisfaction_);
			}
		}
	}

private:
	std::string name_;
	PersonObserver* observer_ = nullptr;
	int satisfaction_ = 100;
	int age_;
	int dance_count_;
};



// Рабочий.
// День рабочего проходит за работой
class Worker : public Person {
public:
	Worker(const std::string& name, int age) : Person(name, age) {}

	// Рабочий старше 30 лет и младше 40 за танец получает 2 единицы удовлетворённости вместо 1
	void Dance() override {
		//Person::Dance();
		if (GetAge() > 30 && GetAge() < 40) {
			SetSatisfaction(GetSatisfaction() + 2);
		}
		else { SetSatisfaction(GetSatisfaction() + 1); }

	}

	// День рабочего проходит за работой
	void LiveADay()  override {
		Person::LiveADay();
		Work();
	}

	// Увеличивает счётчик сделанной работы на 1, уменьшает удовлетворённость на 5
	void Work() {
		SetSatisfaction(GetSatisfaction() - 5);
		work_done_ += 1;
	}

	// Возвращает значение счётчика сделанной работы
	int GetWorkDone() const {
		return work_done_;;
	}
private:
	int work_done_ = 0;
};

// Студент.
// День студента проходит за учёбой
class Student : public Person {
public:
	Student(const std::string& name, int age) : Person(name, age) {}


	// День студента проходит за учёбой
	void LiveADay()  override {
		Person::LiveADay();
		Study();
	}

	// Учёба увеличивает уровень знаний на 1, уменьшает уровень удовлетворённости на 3
	void Study() {
		SetSatisfaction(GetSatisfaction() - 3);
		knowledge_level_ += 1;
	}

	// Возвращает уровень знаний
	int GetKnowledgeLevel() const {
		// Заглушка, напишите реализацию самостоятельно
		return knowledge_level_;
	}
private:
	int knowledge_level_ = 0;
};
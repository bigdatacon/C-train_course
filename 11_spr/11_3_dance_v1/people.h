#pragma once

#include <string>

using namespace std;


class Person;

// ����������� �� ���������� ��������.
class PersonObserver {
public:
	virtual void OnSatisfactionChanged(Person& person, int old_value, int new_value) {
	}
protected:
	// ����� PersonObserver �� ������������ ��� �������� ��������
	~PersonObserver() = default;
};

/*
	�������.
	��� ��������� ������ ���������������� ����������
	���������� � ��� �����������
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

	// ����������� �� 1 ���������� ������� �� �����
	// ����������� ���������������� �� 1
	virtual void Dance() {
		SetSatisfaction(satisfaction_ + 1);
		dance_count_ += 1;
	}

	int GetDanceCount() const {
		return dance_count_;
	}

	// ������� ����. ���������� � ������� ������ ������ �� ������
	virtual void LiveADay() {
		// ��������� ����� �������������� ���� �����
	}
	virtual ~Person() = default;

protected:
	void SetSatisfaction(int value) {
		if (satisfaction_ != value) {
			int old_satisfaction = satisfaction_;
			satisfaction_ = value;
			// ���� � �������� ���� �����������, �������� ��� �� ��������� ����������������
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



// �������.
// ���� �������� �������� �� �������
class Worker : public Person {
public:
	Worker(const std::string& name, int age) : Person(name, age) {}

	// ������� ������ 30 ��� � ������ 40 �� ����� �������� 2 ������� ���������������� ������ 1
	void Dance() override {
		//Person::Dance();
		if (GetAge() > 30 && GetAge() < 40) {
			SetSatisfaction(GetSatisfaction() + 2);
		}
		else { SetSatisfaction(GetSatisfaction() + 1); }

	}

	// ���� �������� �������� �� �������
	void LiveADay()  override {
		Person::LiveADay();
		Work();
	}

	// ����������� ������� ��������� ������ �� 1, ��������� ���������������� �� 5
	void Work() {
		SetSatisfaction(GetSatisfaction() - 5);
		work_done_ += 1;
	}

	// ���������� �������� �������� ��������� ������
	int GetWorkDone() const {
		return work_done_;;
	}
private:
	int work_done_ = 0;
};

// �������.
// ���� �������� �������� �� ������
class Student : public Person {
public:
	Student(const std::string& name, int age) : Person(name, age) {}


	// ���� �������� �������� �� ������
	void LiveADay()  override {
		Person::LiveADay();
		Study();
	}

	// ����� ����������� ������� ������ �� 1, ��������� ������� ���������������� �� 3
	void Study() {
		SetSatisfaction(GetSatisfaction() - 3);
		knowledge_level_ += 1;
	}

	// ���������� ������� ������
	int GetKnowledgeLevel() const {
		// ��������, �������� ���������� ��������������
		return knowledge_level_;
	}
private:
	int knowledge_level_ = 0;
};
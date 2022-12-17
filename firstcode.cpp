#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

// Перечислимый тип для статуса задачи
enum class TaskStatus {
	NEW,          // новая
	IN_PROGRESS,  // в разработке
	TESTING,      // на тестировании
	DONE          // завершена
};

// Объявляем тип-синоним для map<TaskStatus, int>,
// позволяющего хранить количество задач каждого статуса
using TasksInfo = map<TaskStatus, int>;

// cout << TaskStatus.NEW<< endl;

class TeamTasks {
public:
	// Получить статистику по статусам задач конкретного разработчика
	const TasksInfo& GetPersonTasksInfo(const string &person) const {
		return persons_tasks.at(person);

	}
	;

	// Добавить новую задачу (в статусе NEW) для конкретного разработчитка
	void AddNewTask(const string &person) {
		++persons_tasks[person][TaskStatus::NEW];
	}
	;

	template <typename Enumeration>
	auto as_integer(Enumeration const value)
	    -> typename std::underlying_type<Enumeration>::type
	{
	    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
	}

	// Обновить статусы по данному количеству задач конкретного разработчика,
	// подробности см. ниже
	/*tuple<TasksInfo, TasksInfo>*/
	void PerformPersonTasks(const string &person, int task_count) {
		TasksInfo updated_tasks;
		TasksInfo untached_tasks;
		int left_to_change_task = task_count;
		//int counts = task_count;
		for (const auto [status, quantity] : persons_tasks[person]) {
			//cout <<"Persons task quantity: "s << quantity << " status "s<< status <<  endl;
			cout << static_cast<int>(persons_tasks[person][status]) << endl;
			//
		}
		TasksInfo persons_tasks_statuses = persons_tasks[person];
		cout << persons_tasks_statuses[(TaskStatus) (0)] << endl;
		cout << persons_tasks_statuses[TaskStatus::NEW] << endl;
		cout << persons_tasks_statuses[TaskStatus::NEW] << endl;
		cout << static_cast<int>(persons_tasks[person][TaskStatus::NEW])
				<< endl;
		cout << static_cast<int>(persons_tasks[person][(TaskStatus) (1)])
				<< endl;

		//cout << static_cast<int>(static_cast<int>(persons_tasks[person][TaskStatus::NEW]))== task_count << endl;
		//cout << static_cast<int>(static_cast<int>(persons_tasks.at(person)[TaskStatus::NEW]))== task_count << endl;
		//cout << static_cast<int>(persons_tasks.at(person)[TaskStatus::NEW])== static_cast<int>(task_count) << endl;
		cout << (static_cast<int>(static_cast<int>(persons_tasks.at(person)[TaskStatus::NEW]))== task_count) << endl;
		cout << (static_cast<int>(static_cast<int>(persons_tasks.at(person)[(TaskStatus)(0)]))!= task_count) << endl;
		//cout << typeid(TaskStatus::NEW).name() << endl;
		//cout << typeid((static_cast<int>(static_cast<int>(persons_tasks.at(person)[TaskStatus::TESTING]))== task_count)).name() << endl;

		//cout << typeid(persons_tasks.at(person)[TaskStatus]).name() << endl;
		//cout << get_next_status(persons_tasks[person][TaskStatus::NEW])<< endl;
		for(const auto [status, quantity] : persons_tasks.at(person)){
			//cout << typeid(status).name() <<  endl;
			TaskStatus a = TaskStatus::NEW;
			//cout << status <<  endl;
			std::cout <<"here "s << as_integer(a) << std::endl;
		}
		
		

		// Алгоритм
		/*for (const auto [status, quantity] : persons_tasks[person]) {
			if (left_to_change_task >= quantity) {
				left_to_change_task = left_to_change_task - quantity;
				updated_tasks[person][static_cast<int>(status) + 1] += quantity;

			} else {

				updated_tasks[person][static_cast<int>(status)] +=
						left_to_change_task;
				untached_tasks[person][static_cast<int>(status)] += (quantity
						- left_to_change_task);
				left_to_change_task = 0;
			}
		}*/

	}
	;

//    New : 2
//	IN_PROGRESS : 1
//	TESTING : 1
//	DONE : 1

private:
// храним имя разработчика и статусы+количество его задач
	map<string, TasksInfo> persons_tasks;

	/*TaskStatus get_next_status(TaskStatus status) {
		if (status == TasksInfo.NEW) {
			return TasksInfo.IN_PROGRESS;
		} else if (status == TasksInfo.IN_PROGRESS) {
			return TasksInfo.TESTING;
		} else if (status == TasksInfo.TESTING) {
			return false;
		}}*/

	TaskStatus get_next_status(TaskStatus status) {
		if (status == TaskStatus::NEW) {
			return TaskStatus::IN_PROGRESS;
		} else if (status == TaskStatus::IN_PROGRESS) {
			return TaskStatus::TESTING;
		} else if (status == TaskStatus::TESTING) {
			return status;
		}}
}
		;

/////////////////////////////////////////////////////////////////////////НЕ ОТНОСИТСЯ ////////////////////////////////

// Принимаем словарь по значению, чтобы иметь возможность
// обращаться к отсутствующим ключам с помощью [] и получать 0,
// не меняя при этом исходный словарь.
void PrintTasksInfo(TasksInfo tasks_info) {
	cout << tasks_info[TaskStatus::NEW] << " new tasks"s << ", "s
			<< tasks_info[TaskStatus::IN_PROGRESS] << " tasks in progress"s
			<< ", "s << tasks_info[TaskStatus::TESTING]
			<< " tasks are being tested"s << ", "s
			<< tasks_info[TaskStatus::DONE] << " tasks are done"s << endl;
}

int main() {
	TeamTasks tasks;
	tasks.AddNewTask("Ilia"s);
	for (int i = 0; i < 3; ++i) {
		tasks.AddNewTask("Ivan"s);
	}
	cout << "Ilia's tasks: "s;
	PrintTasksInfo(tasks.GetPersonTasksInfo("Ilia"s));
	cout << "Ivan's tasks: "s;
	PrintTasksInfo(tasks.GetPersonTasksInfo("Ivan"s));

	tasks.PerformPersonTasks("Ivan"s, 2);

	//tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan"s, 2);

	/*TasksInfo updated_tasks, untouched_tasks;

	 tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan"s, 2);
	 cout << "Updated Ivan's tasks: "s;
	 PrintTasksInfo(updated_tasks);
	 cout << "Untouched Ivan's tasks: "s;
	 PrintTasksInfo(untouched_tasks);

	 tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan"s, 2);
	 cout << "Updated Ivan's tasks: "s;
	 PrintTasksInfo(updated_tasks);
	 cout << "Untouched Ivan's tasks: "s;
	 PrintTasksInfo(untouched_tasks);*/
}


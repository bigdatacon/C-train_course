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

	template<typename Enumeration>
	auto as_integer(
			Enumeration const value) -> typename std::underlying_type<Enumeration>::type {
		return static_cast<typename std::underlying_type<Enumeration>::type>(value);
	}

	// Обновить статусы по данному количеству задач конкретного разработчика,
	// подробности см. ниже
	/*tuple<TasksInfo, TasksInfo>*/
	void PerformPersonTasks(const string &person, int task_count) {
		int left_to_change_task = task_count;
		int status_number = 0;
		int next_status = status_number + 1;

		cout
				<< (static_cast<int>(static_cast<int>(persons_tasks.at(person)[(TaskStatus) (status_number)])))
				<< endl;
		persons_tasks.at(person)[(TaskStatus) (status_number)] += 4;
		cout
				<< (static_cast<int>(static_cast<int>(persons_tasks.at(person)[(TaskStatus) (status_number)])))
				<< endl;

		// ниже закомментировано - не работает - не пойму почему
		//updated_tasks[person][status_number] += 4;
		//cout << (static_cast<int>(static_cast<int>(updated_tasks.at(person)[(TaskStatus)(status_number)]))) << endl;

		for (const auto [status, quantity] : persons_tasks.at(person)) {

			if (left_to_change_task >= quantity) {

				left_to_change_task = left_to_change_task - quantity;
				AddNewTaskUpdated(person, next_status, quantity);

			} else {
				AddNewTaskUpdated(person, next_status, left_to_change_task);
				int itg_q = quantity - left_to_change_task;
				AddNewTaskUntached(person, status, itg_q);
				left_to_change_task = 0;

			}
			++status_number;
			++next_status;
		}

		// удаляю из нетронутых статус DONE  - ниже 2 варианта Вы могли бы уточнить как правильно и в чем разница между at - он исключение выдаст если нет ключа?
		// но почему тогда в цикле все через .at()?
		untached_tasks[person].erase(TaskStatus::DONE);
		untached_tasks.at(person).erase(TaskStatus::DONE);

	}
	;

private:
// храним имя разработчика и статусы+количество его задач
	map<string, TasksInfo> persons_tasks;
	map<string, TasksInfo> updated_tasks;
	map<string, TasksInfo> untached_tasks;

	void AddNewTaskUpdated(const string &person, int status, int quantity) {
		if (status == 0) {
			updated_tasks[person][TaskStatus::NEW] += quantity;
		} else if (status == 1) {
			updated_tasks[person][TaskStatus::IN_PROGRESS] += quantity;
		}

		else if (status == 2) {
			updated_tasks[person][TaskStatus::TESTING] += quantity;
		}

		else if (status == 3) {
			updated_tasks[person][TaskStatus::DONE] += quantity;
		}
		else if (status == 4) {
			return;
		}
		
	}

	void AddNewTaskUntached(const string &person, int status, int quantity) {
		if (status == 0) {
			untached_tasks[person][TaskStatus::NEW] += quantity;
		} else if (status == 1) {
			untached_tasks[person][TaskStatus::IN_PROGRESS] += quantity;
		}

		else if (status == 2) {
			untached_tasks[person][TaskStatus::TESTING] += quantity;
		}

		else if (status == 3) {
			untached_tasks[person][TaskStatus::DONE] += quantity;
		}
	}
	;

	/*TaskStatus get_next_status(TaskStatus status) {
	 if (status == TasksInfo.NEW) {
	 return TasksInfo.IN_PROGRESS;
	 } else if (status == TasksInfo.IN_PROGRESS) {
	 return TasksInfo.TESTING;
	 } else if (status == TasksInfo.TESTING) {
	 return status;
	 }}*/

	TaskStatus get_next_status(TaskStatus status) {
		if (status == TaskStatus::NEW) {
			return TaskStatus::IN_PROGRESS;
		} else if (status == TaskStatus::IN_PROGRESS) {
			return TaskStatus::TESTING;
		} else if (status == TaskStatus::TESTING) {
			return status;
		}
	}
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

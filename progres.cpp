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

	tuple<TasksInfo, TasksInfo> PerformPersonTasks(const string &person,
			int task_count) {
		int left_to_change_task = task_count;
		int status_number = 0;
		int next_status = status_number + 1;

		for (const auto [status, quantity] : untached_tasks[person]) {
			cout << " BEGIN quantity_untached_tasks :  " << quantity << endl;
		}
		for (const auto [status, quantity] : updated_tasks[person]) {
			cout << " BEGIN quantity_updated_tasks :  " << quantity << endl;
		}

		for (const auto [status, quantity] : persons_tasks.at(person)) {
			cout <<"in NAME :  "s << person << "  input left_to_change_task : " << left_to_change_task << " input quantity : " << quantity << " input status_number : "s
			 << status_number << " input next_status : "s << next_status << endl;

			if (left_to_change_task >= quantity) {
				AddNewTaskUpdated(person, next_status, quantity);
				left_to_change_task = left_to_change_task - quantity;
				persons_tasks.at(person)[(TaskStatus) (status_number)] -=
						quantity;
				persons_tasks.at(person)[(TaskStatus) (next_status)] +=
						quantity;

			} else {
				if (left_to_change_task != 0) {
					if (quantity != 0) {
						AddNewTaskUpdated(person, next_status,
								left_to_change_task);
						int itg_q = quantity - left_to_change_task;
						AddNewTaskUntached(person, status_number, itg_q);
						left_to_change_task = 0;
						persons_tasks.at(person)[(TaskStatus) (status_number)] -=
								itg_q;
						persons_tasks.at(person)[(TaskStatus) (next_status)] +=
								itg_q;

					}
				 else {
					continue;
				}
				}
			  else break;

		}
		++status_number;
		++next_status;
		cout << "out NAME :  "s << person << "  out left_to_change_task : " << left_to_change_task << " out quantity : " << quantity << " out status_number : "s
		 << status_number << " out next_status : "s << next_status << endl;
	}

		untached_tasks[person].erase(TaskStatus::DONE);

		for (const auto [status, quantity] : untached_tasks[person]){cout << " END quantity_untached_tasks :  " << quantity << endl; }
		for (const auto [status, quantity] : updated_tasks[person]){cout << " END quantity_updated_tasks :  " << quantity << endl; }

		return make_tuple(updated_tasks[person], untached_tasks[person]);

	}
	;

private:
// храним имя разработчика и статусы+количество его задач
	map<string, TasksInfo> persons_tasks;
	map<string, TasksInfo> updated_tasks;
	map<string, TasksInfo> untached_tasks;

	/*void AddNewTaskPerson(const string &person, int status, int quantity) {
		if (status == 0) {
			persons_tasks[person][TaskStatus::NEW] += quantity;
		} else if (status == 1) {
			persons_tasks[person][TaskStatus::IN_PROGRESS] += quantity;
		}

		else if (status == 2) {
			persons_tasks[person][TaskStatus::TESTING] += quantity;
		}

		else if (status == 3) {
			persons_tasks[person][TaskStatus::DONE] += quantity;
		}
	}
	;*/

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
    tasks.AddNewTask("Ilia");
    for (int i = 0; i < 3; ++i) {
        tasks.AddNewTask("Ivan");
    }
    cout << "Ilia's tasks: ";
    PrintTasksInfo(tasks.GetPersonTasksInfo("Ilia"));
    cout << "Ivan's tasks: ";
    PrintTasksInfo(tasks.GetPersonTasksInfo("Ivan"));

    TasksInfo updated_tasks, untouched_tasks;

    tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan", 2);
    cout << "Updated Ivan's tasks: ";
    PrintTasksInfo(updated_tasks);
    cout << "Untouched Ivan's tasks: ";
    PrintTasksInfo(untouched_tasks);

    tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan", 2);
    cout << "Updated Ivan's tasks: ";
    PrintTasksInfo(updated_tasks);
    cout << "Untouched Ivan's tasks: ";
    PrintTasksInfo(untouched_tasks);
}

// правильный вывод
/*
 Ilia's tasks: 1 new tasks, 0 tasks in progress, 0 tasks are being tested, 0 tasks are done
Ivan's tasks: 3 new tasks, 0 tasks in progress, 0 tasks are being tested, 0 tasks are done
Updated Ivan's tasks: 0 new tasks, 2 tasks in progress, 0 tasks are being tested, 0 tasks are done
Untouched Ivan's tasks: 1 new tasks, 0 tasks in progress, 0 tasks are being tested, 0 tasks are done
Updated Ivan's tasks: 0 new tasks, 1 tasks in progress, 1 tasks are being tested, 0 tasks are done
Untouched Ivan's tasks: 0 new tasks, 1 tasks in progress, 0 tasks are being tested, 0 tasks are done
 */


// мой вывод
/*
Ilia's tasks: 1 new tasks, 0 tasks in progress, 0 tasks are being tested, 0 tasks are done
Ivan's tasks: 3 new tasks, 0 tasks in progress, 0 tasks are being tested, 0 tasks are done
Updated Ivan's tasks: 0 new tasks, 2 tasks in progress, 0 tasks are being tested, 0 tasks are done
Untouched Ivan's tasks: 1 new tasks, 0 tasks in progress, 0 tasks are being tested, 0 tasks are done
Updated Ivan's tasks: 0 new tasks, 4 tasks in progress, 0 tasks are being tested, 0 tasks are done
Untouched Ivan's tasks: 1 new tasks, 0 tasks in progress, 0 tasks are being tested, 0 tasks are done

  */

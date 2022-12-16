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

class TeamTasks {
public:
    // Получить статистику по статусам задач конкретного разработчика
    const TasksInfo& GetPersonTasksInfo(const string& person) const{
    	return persons_tasks.at(person);

    };

    // Добавить новую задачу (в статусе NEW) для конкретного разработчитка
    void AddNewTask(const string& person){
    	++persons_tasks[person][TaskStatus::NEW];
    };

    // Обновить статусы по данному количеству задач конкретного разработчика,
    // подробности см. ниже
    /*tuple<TasksInfo, TasksInfo>*/ void PerformPersonTasks(const string& person , int task_count){
    	TasksInfo updated_tasks;
    	TasksInfo untached_tasks;
    	//int counts = task_count;
        for(const auto [status, quantity] : persons_tasks[person])
        {
        	//cout <<"Persons task quantity: "s << quantity << " status "s<< status <<  endl;
        	cout << static_cast<int>(persons_tasks[person][status])<< endl;
        	cout << static_cast<int>(persons_tasks[person][static_cast<int>(status)+1])<< endl;
        }
        TasksInfo persons_tasks_statuses  = persons_tasks[person];
        cout << persons_tasks_statuses[(TaskStatus)(0)]<< endl;
        cout << persons_tasks_statuses[TaskStatus::NEW]<< endl;
        cout << persons_tasks_statuses[TaskStatus::NEW]<< endl;
        cout << static_cast<int>(persons_tasks[person][TaskStatus::NEW])<< endl;
        cout << static_cast<int>(persons_tasks[person][(TaskStatus)(1)])<< endl;

        cout << static_cast<int>(persons_tasks[person][TaskStatus::NEW]) ==  task_count << endl;


    };


private:
// храним имя разработчика и статусы+количество его задач
map<string, TasksInfo> persons_tasks;
};



/////////////////////////////////////////////////////////////////////////НЕ ОТНОСИТСЯ ////////////////////////////////

// Принимаем словарь по значению, чтобы иметь возможность
// обращаться к отсутствующим ключам с помощью [] и получать 0,
// не меняя при этом исходный словарь.
void PrintTasksInfo(TasksInfo tasks_info) {
    cout << tasks_info[TaskStatus::NEW] << " new tasks"s
         << ", "s << tasks_info[TaskStatus::IN_PROGRESS] << " tasks in progress"s
         << ", "s << tasks_info[TaskStatus::TESTING] << " tasks are being tested"s
         << ", "s << tasks_info[TaskStatus::DONE] << " tasks are done"s << endl;
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


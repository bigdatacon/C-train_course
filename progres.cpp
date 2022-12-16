#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>
#include <algorithm>
#include <numeric>
 
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
    private:
    // храним имя разработчика и статусы+количество его задач
    map<string, TasksInfo> m_persons_tasks;
 
public:
    // Получить статистику по статусам задач конкретного разработчика
    const TasksInfo& GetPersonTasksInfo(const string& person) const
    {
        return m_persons_tasks.at(person);
    }
 
    // Добавить новую задачу (в статусе NEW) для конкретного разработчитка
    void AddNewTask(const string& person)
    {
        ++m_persons_tasks[person][TaskStatus::NEW];
    }
 
    // Обновить статусы по данному количеству задач конкретного разработчика,
    // подробности см. ниже
    tuple<TasksInfo, TasksInfo> PerformPersonTasks(const string& person, int task_count)
    {
        tuple<TasksInfo, TasksInfo> personInfo;
        // нет такого разработчика или у него нет задач
        if(m_persons_tasks.find(person) == m_persons_tasks.end() || m_persons_tasks[person].empty())
            return personInfo;
 
        // скопируем данные о задачах
        TasksInfo ti_tmp = m_persons_tasks[person];
        TasksInfo ti_untouched = ti_tmp;
        // удалим из словаря нетронутых - статусы DONE
        ti_untouched.erase(TaskStatus::DONE);
        TasksInfo ti_updated;
 
        // посчитаем невыполненные задачи
        int unfulfilled_tasks = 0;
        for(const auto [status, quantity] : m_persons_tasks[person])
        {
            if(status == TaskStatus::DONE)
                break;
            unfulfilled_tasks += quantity;
        }
 
        for(const auto [status, quantity] : m_persons_tasks[person])
        {
            if(quantity == 0) // нет задач в данном статусе
                continue;
            if(status == TaskStatus::DONE) // не работаем с данными статусами
                break;
            // если
            if(task_count == 0)
                break;
            if(task_count > unfulfilled_tasks)
                task_count = unfulfilled_tasks;
            if(quantity >= task_count)
            {
                ti_tmp[status] -= task_count;
                ti_tmp[(TaskStatus)(static_cast<int>(status)+1)] += task_count;
                ti_updated[(TaskStatus)(static_cast<int>(status)+1)] = task_count;
                ti_untouched[status] -= task_count;
                break;
            }
            else
            {
                ti_tmp[status] = 0;
                ti_tmp[(TaskStatus)(static_cast<int>(status)+1)] += quantity;
                ti_updated[(TaskStatus)(static_cast<int>(status)+1)] = quantity;
                ti_untouched[status] = ti_tmp[status];
                task_count -= quantity;
            }
        }
        m_persons_tasks[person] = ti_tmp;
 
        ti_tmp = ti_updated;
        for(const auto [status, quantity] : ti_updated)
        {
            if(quantity == 0)
                ti_tmp.erase(status);
        }
 
        ti_tmp = ti_untouched;
        for(const auto [status, quantity] : ti_untouched)
        {
            if(quantity == 0)
                ti_tmp.erase(status);
        }
 
        return tuple(ti_updated, ti_untouched);
    }
 
    void Dan()
    {
        string key = "Dan"s;
        m_persons_tasks[key][TaskStatus::NEW] = 3;
        m_persons_tasks[key][TaskStatus::IN_PROGRESS] = 2;
        m_persons_tasks[key][TaskStatus::TESTING] = 4;
        m_persons_tasks[key][TaskStatus::DONE] = 1;
    }
 
};
 
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
    TasksInfo updated_tasks, untouched_tasks;
    tasks.Dan();
 
    cout << "Dan tasks"s << endl;
    PrintTasksInfo(tasks.GetPersonTasksInfo("Dan"));
 
    tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Dan"s, 4);
    cout << "Updated Dan's tasks: "s;
    PrintTasksInfo(updated_tasks);
    cout << "Untouched Dan's tasks: "s;
    PrintTasksInfo(untouched_tasks);
    cout << "---------------------"s << endl;
 
    tasks.AddNewTask("Ilia"s);
    for (int i = 0; i < 3; ++i) {
        tasks.AddNewTask("Ivan"s);
    }
    cout << "Ilia's tasks: "s;
    PrintTasksInfo(tasks.GetPersonTasksInfo("Ilia"s));
    cout << "Ivan's tasks: "s;
    PrintTasksInfo(tasks.GetPersonTasksInfo("Ivan"s));
 
 
 
    tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan"s, 1000);
    cout << "Updated Ivan's tasks: "s;
    PrintTasksInfo(updated_tasks);
    cout << "Untouched Ivan's tasks: "s;
    PrintTasksInfo(untouched_tasks);
 
    tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan"s, 2);
    cout << "Updated Ivan's tasks: "s;
    PrintTasksInfo(updated_tasks);
    cout << "Untouched Ivan's tasks: "s;
    PrintTasksInfo(untouched_tasks);
 
    tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan"s, 2);
    cout << "Updated Ivan's tasks: "s;
    PrintTasksInfo(updated_tasks);
    cout << "Untouched Ivan's tasks: "s;
    PrintTasksInfo(untouched_tasks);
 
    tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan"s, 2);
    cout << "Updated Ivan's tasks: "s;
    PrintTasksInfo(updated_tasks);
    cout << "Untouched Ivan's tasks: "s;
    PrintTasksInfo(untouched_tasks);
 
    tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan"s, 1000);
    cout << "Updated Ivan's tasks: "s;
    PrintTasksInfo(updated_tasks);
    cout << "Untouched Ivan's tasks: "s;
    PrintTasksInfo(untouched_tasks);
}
 

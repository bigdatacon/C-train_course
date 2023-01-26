#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;
/*
Иногда экспонента всё-таки необходима. Реализуйте брутфорс для подбора пароля к секретному объекту Неверляндии. К счастью, контрразведка Неверляндии не очень продвинута — Неверландский системный администратор Артур установил пароль, содержащий всего пять латинских букв верхнего регистра A-Z. Вам не составит труда подобрать его. Функция проверки пароля будет передана в качестве аргумента.
Ограничения
Не меняйте сигнатуру функции BruteForce. 
Пример
Если пароль ABCDE, то check("ABCDE"s) == true, check(str) == false, где str — любая другая строка.
Что отправлять на проверку
Включите в решение реализацию функции BruteForce. Функция main при проверке будет игнорироваться.
Как будет тестироваться ваш код
Функция BruteForce будет вызываться с различными предикатами check. Для каждого гарантируется, что есть ровно одна строка из пяти символов A-Z, на которой он истинен.
Вы можете выбрать один из двух подходов к решению.
Простой. Реализуйте 5 вложенных циклов по char от 'A' до 'Z'.
Универсальный. Годится, если длина пароля заранее неизвестна. Напишите рекурсивную функцию string BruteForceInternal(F check, const string& begin, int n). При n == 5 она будет вызывать check(begin) и возвращать результат вызова в случае успеха, пустую строку в случае неудачи. При n < 5 она будет вызывать себя, приписывая к begin всевозможные символы и увеличивая n на 1. Если рекурсивный вызов BruteForceInternal вернул непустую строку, то это и есть правильный ответ, дальнейший перебор не нужен.
*/

// -- реализация через цикл 
template <typename F>
string BruteForce(F check) {
    // верните строку str, для которой check(str) будет true
    for (char one =  'A'; one <= 'Z'; ++one){
        for (char two =  'A'; one <= 'Z'; ++one){
            for (char three =  'A'; one <= 'Z'; ++one){
                for (char four =  'A'; one <= 'Z'; ++one){
                    for (char five =  'A'; one <= 'Z'; ++one){
                            string s{one, two, three, four, five};
                            if (!check(s)){continue;}
                            else {return s;}

    
                        
                    }
                    
                }
                
            }
            
            
        }
    }
    return ""s;
}



// -- реализация через рекурсию
template <typename F>
string BruteForce(F check) {
    // верните строку str, для которой check(str) будет true
    char one =  'A' + rand() % 26;
    char two =  'A' + rand() % 26;
    char three =  'A' + rand() % 26;
    char four =  'A' + rand() % 26;
    char five =  'A' + rand() % 26;
    string s{one, two, three, four, five};

    if (!check(s)){BruteForce(check);}
    return s;
}


// - тут заготовка которую я не правил 
/*template <typename F>
string BruteForce(F check) {
    // верните строку str, для которой check(str) будет true
}*/

int main() {
    string pass = "ARTUR"s;
    auto check = [pass](const string& s) {
        return s == pass;
    };
    cout << BruteForce(check) << endl;
}

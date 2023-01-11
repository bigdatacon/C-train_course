
#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace std;
/* самостоятельно напишите код, требуемый по условию задачи */
/*
Задание 2
Напишите рекурсивную функцию IsPowOfTwo, которая возвращает true, если число — неотрицательная степень двойки, или false, если оно таковым не будет. Операцией возведения в степень в этой задаче пользоваться нельзя. Единица является нулевой степенью двойки ($2^0 = 1)$, и ответ должен быть true.
Пример использования
int main() {
    int result = IsPowOfTwo(1024);
    cout << result << endl;
} 
Пример вывода
В случае true на экран выводится 1, в случае false — 0. В нашем случае 1024 — это  степень двойки.
1 

Подсказка 
Какие параметры принимает функция? Проверяемое число.
Какие действия должна совершить функция на каждом шаге? Если число — степень двойки, оно должно делиться на два. Результат деления тоже должен быть степенью двойки. Проверить остаток от деления на два можно, используя оператор %.
Каково условие завершения? Действуйте аналогично предыдущей задаче.

*/



#include <iostream>

using namespace std;


uint64_t Factorial(int num) {
    int factorial = 1;
    if (0 != num) {
        factorial = Factorial(num - 1) * num;
    }
    return factorial;
} 

/*int Fibonacci(int i) {
    if (0 == i) {
        return 0;
    }
    if (1 == i) {
        return 1;
    }
    return Fibonacci(i - 1) + Fibonacci(i - 2);
}*/


bool IsPowOfTwo(int i) {
    bool res = i % 2 == 0;
    bool ispow = false;
     if (i<=0) {
        return false;
    }
    if (!res) {return false;} // проверка что числе вообще четное и целое 
    if (1 == i) {
        return true;
    }
    
    if (i > 1 && res ){ ispow =  (i/2) %2 ==0; }
    return ispow;
    

}




int main() {
    int result = IsPowOfTwo(1024);
    cout << result << endl;
}

#include <iostream>
#include <string>

/*
Подсказка:
Можете использовать хешер окружности из урока. Хешер текста — стандартный.
Объединять хеши компонентов структуры нужно с умом, чтобы использовались разные степени числа 37 при разных компонентах: хеш первой окружности должен умножаться на 37⁴, хеш второй — на 37¹ или 37, а хеш строки — на 37⁰ или 1.

Задание 2
Реализуйте хороший хешер для структуры Dumbbell, состоящей из двух окружностей и текста.
struct Circle {
    double x;
    double y;
    double r;
};

struct Dumbbell {
    Circle circle1;
    Circle circle2;
    string text;
}; 
Используйте для вычисления хеша степени простого числа, но учтите: никакие два параметра не должны умножаться на одну и ту же степень, иначе хеш-функция получится некачественной.
Ограничения
Не меняйте название класса хешера DumbbellHash. Не меняйте структуры Dumbbell и Circle.
Как будет тестироваться ваш код
Проверим качество вашей хеш-функции.


*/


using namespace std;

struct Circle {
    double x;
    double y;
    double r;
};

struct Dumbbell {
    Circle circle1;
    Circle circle2;
    string text;
};

struct DumbbellHash {
// реализуйте хешер для Dumbbell
};

int main() {
    DumbbellHash hash;
    Dumbbell dumbbell{{10, 11.5, 2.3}, {3.14, 15, -8}, "abc"s};
    cout << "Dumbbell hash "s << hash(dumbbell);

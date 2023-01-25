#include "log_duration.h"

#include <iostream>

using namespace std;

/*
Задание
Разработайте класс StreamUntier, который выполняет следующие действия:
В конструкторе — «отвязывание» потока, переданного в конструктор как аргумент.
В деструкторе — «привязывание» того потока, который был отвязан.
В этом уроке вы столкнулись с указателями — объектами C++, которые изучите позже в курсе. Тип указателя содержит звёздочку. Вы видели такой тип в параметре main в уроке. Метод потока tie тоже оперирует с указателем — ostream*. Именно им был тип переменной tied_before, выведенный автоматически в выражении auto tied_before = cin.tie(nullptr);.
Интерфейс класса StreamUntier дан в заготовке кода. Добавьте конструктор, деструктор и, при необходимости, дополнительные поля.

Выполните stream.tie(nullptr) в конструкторе, сохранив возвращённое значение в поле tied_before_. Используйте его в деструкторе. Придётся также добавить поле класса типа istream& для того, чтобы сохранить сам stream.
*/


class StreamUntier {
public:
    // добавьте конструктор, деструктор
    // и дополнительные поля класса при необходимости
        StreamUntier(istream& stream) : stream_(stream) {
           ostream* tied_before_ = stream.tie(nullptr);
    }

    ~StreamUntier() {

    }


private:
    ostream* tied_before_;
    istream& stream_;
};

int main() {
    LOG_DURATION("\\n with tie"s);

    StreamUntier guard(cin);
    int i;
    while (cin >> i) {
        cout << i * i << "\n"s;
    }

    return 0;
}

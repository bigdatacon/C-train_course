#include <cassert>
#include <string>


/*
Задание 2
Напишите шаблонную функцию IsSameObject, проверяющую, ссылаются ли переданные ей параметры-ссылки на один и тот же объект.
Ограничения
Не меняйте сигнатуру функции IsSameObject.
Как будет тестироваться ваш код
Будет проверена работа функции IsSameObject. Ей может быть передана пара любых объектов одного и того же типа.
Содержимое функции main будет проигнорировано.
Что отправлять на проверку
Отправьте код функции IsSameObject.


Две ссылки относятся к одному и тому же объекту, только если адреса их объектов совпадают.
Узнать адрес объекта по ссылке можно операцией &.
Напишите сигнатуру функции так, чтобы значения в неё всегда передавались по ссылке.
*/

//изначальный код 
template </* ваша реализация */>
bool IsSameObject() {

/* ваша реализация */
}

// Моя реалищзация  ниже 
template <typename T>
bool IsSameObject(T& value1, T& value2) {
    /*Напишите тело функции самостоятельно */
    //T* value_ptr1 = &value1;
    //T* value_ptr2 = &value2;
    
    T* value_ptr1 = value1;
    T* value_ptr2 = value2;

    // Значения указателей на один и тот же объект будут равны
    return assert(value_ptr1 == value_ptr2);
    
}

int main() {
    using namespace std;

    const int x = 1;
    const int y = 1;
    // x и y — разные объекты, хоть и имеющие одинаковое значение
    assert(!IsSameObject(x, y));
    // Оба аргумента — один и тот же объект
    assert(IsSameObject(x, x));

    const string name1 = "Harry"s;
    const string name1_copy = name1;
    const string name2 = "Ronald"s;
    auto name1_ptr = &name1;
    const string& name1_ref = name1;

    assert(!IsSameObject(name1, name2));  // Две строки с разными значениями — разные объекты
    assert(!IsSameObject(name1, name1_copy));  // Строка и её копия — разные объекты

    // Оба параметра ссылаются на одну и ту же строку
    assert(IsSameObject(name1, name1));
    assert(IsSameObject(name2, name2));
    assert(IsSameObject(name1_copy, name1_copy));

    // Разыменованный указатель на объект и сам объект — один и тот же объект
    assert(IsSameObject(*name1_ptr, name1));

    // Переменная и ссылка на неё относятся к одному и тому же объекту
    assert(IsSameObject(name1_ref, name1));
    // Ссылка на объект и разыменованный указатель на объект относятся к одному и тому же объекту
    assert(IsSameObject(name1_ref, *name1_ptr));
}

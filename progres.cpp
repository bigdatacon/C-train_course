#include <iostream>
#include <string>
#include <set>
using namespace std;

/*
Разработайте шаблонную функцию RunTestImpl и использующий её макрос RUN_TEST. Они должны запускать тесты и выводить сообщения об их завершении в стандартный поток ошибок.
*/

/*
Чтобы узнать имя функции, получите строковое представление параметра func так же, как получаете строковое значение аргументов в макросе ASSERT_EQUAL. Передайте его в функцию RunTestImpl вторым параметром. В качестве первого параметра функция RunTestImpl должна принимать функцию-тест. Для этого функцию RunTestImpl сделайте шаблонной — компилятор сможет самостоятельно вывести тип первого аргумента.
Чтобы проверить, как работает фреймворк, попробуйте внести логические ошибки в методы класса Synonyms, например:
*/


template <typename T>
void RunTestImpl(const T& t, const string& func_str, const string& file, const string& func, unsigned line) {
    /* Напишите недостающий код */
    
}

/**#define ASSERT(expr) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, ""s)
#define ASSERT_HINT(expr, hint) AssertImpl(!!(expr), #expr, __FILE__, __FUNCTION__, __LINE__, (hint))*/

#define RUN_TEST(func) RunTestImpl(func, #func, __FILE__, __FUNCTION__, __LINE__)  // напишите недостающий код

void Test1() {
        int a = 5;
        int b = 6;
        if (a!=b) {cerr << "OK"s; };
        abort();
}

int main() {
    RUN_TEST(Test1);
}

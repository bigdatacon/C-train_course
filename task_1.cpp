
#include <cstdlib>
#include <iomanip>  // Подключаем, чтобы использовать манипулятор boolalpha
#include <iostream>
#include <string>

using namespace std;

template <typename T, typename U>
void Assert(const T& t, const U& u, const string& hint) {
    if (t != u) {
        // Включаем режим вывода значений типа bool в виде true и false
        cout << boolalpha;
        cout << "Assertion failed: "s << t << " != "s << u << "."s;
        if (!hint.empty()) {
            cout << " Hint: "s << hint;
        }
        cout << endl;
        abort();
    }
}




int main() {
    const string greeting = "Hello"s;
    // Намеренная ошибка в условии, чтобы показать работу Assert
    Assert(greeting.empty(), "Greeting must be non-empty"s);
    // Следующая строка не должна выполниться, так как Assert аварийно завершит работу программы
    cout << "This line will not be printed"s << endl;
} 

#include <numeric>
#include <ostream>
#include <istream>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>

using namespace std;
/*Задание
Реализуйте для класса Rational операции ввода и вывода в формате числитель/знаменатель. Гарантируется, что данные во входном потоке будут корректными.
Не забудьте в операциях ввода и вывода возвращать ссылку на поток, переданный в качестве первого аргумента. Это позволит объединить несколько операций вывода в цепочки:
cout << rational1 << " "s << rational2; 
*/


class Rational {
public:
    Rational() = default;

    Rational(int numerator)
        : numerator_(numerator)
        , denominator_(1) {
    }

    Rational(int numerator, int denominator)
        : numerator_(numerator)
        , denominator_(denominator) {
        Normalize();
    }

    int Numerator() const {
        return numerator_;
    }

    int Denominator() const {
        return denominator_;
    }

private:
    void Normalize() {
        if (denominator_ < 0) {
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }
        const int divisor = gcd(numerator_, denominator_);
        numerator_ /= divisor;
        denominator_ /= divisor;
    }

    int numerator_ = 0;
    int denominator_ = 1;
};

// вывод
ostream& operator<<(ostream& output, Rational& r) {
    //char slash = "/"s;  - так не работает 
    //output << r.Numerator() << slash << r.Denominator();
    output << r.Numerator() << "/" << r.Denominator();  // так не проходит проверку 
    
    return output;
}

// ввод
istream& operator>>(istream& input, Rational& r) {
    int numerator, denominator;
    input >> numerator >> denominator;
    r = Rational{numerator, denominator};
    return input;
} 


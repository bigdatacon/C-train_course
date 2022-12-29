#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>

using namespace std;

/*
Перед приведением дроби к несократимому виду измените знак числителя и знаменателя, если знаменатель изначально был отрицательным. Затем и числитель, и знаменатель разделите на их наибольший общий делитель. Для нахождения наибольшего общего делителя в C++ есть удобная функция gcd из заголовочного файла <numeric>. Вы можете познакомиться с ней в документации по С++ или разработать такую функцию самостоятельно.
*/

class Rational {
public:
    
            // Явно сообщаем компилятору, что в этом классе нужно создать конструктор по умолчанию
    Rational() = default;
    
        // Параметризованный конструктор
    Rational(int numerator) {
        numerator_ = numerator;
        denominator_ = 1;  
    }
    
    // Параметризованный конструктор
    Rational(int numerator, int denominator ) {
        int num_input = numerator;
        int denom_input = denominator;       
        if (denom_input<0){num_input = -num_input;}
        
        int deviser = gcd(num_input, denom_input);
        numerator_ = num_input/deviser;
        denominator_ = denom_input/deviser;
    }
    
    int Numerator() const {
        return numerator_;
    }

    int Denominator() const {
        return denominator_;
    }

private:
    int numerator_ = 0;
    int denominator_ = 1;
};


Rational Add(Rational r1, Rational r2) {
    int numerator = r1.Numerator() * r2.Denominator() + r2.Numerator() * r1.Denominator();
    int denominator = r1.Denominator() * r2.Denominator();
    // Создаём и возвращаем дробь с заданным числителем и знаменателем
    return Rational{numerator, denominator};
}
int main() {
    Rational zero;     // Дробь 0/1 = 0
    const Rational seven(7); // Дробь 7/1 = 7
    const Rational one_third(1, 3); // Дробь 1/3
    vector<Rational> numbers;
    numbers.push_back(Rational{7, 8});
    // Следующие 2 строки эквивалентны - добавляют в numbers дробь 3/1
    numbers.push_back(Rational{3});
    numbers.push_back(3);
    Rational sum = Add(Rational{1,6}, one_third);
    cout << "SUM : " << sum.Numerator() << endl;
    // Выведет 1/2

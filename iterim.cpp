#include <iostream>
#include <numeric>

using namespace std;

/*
Задание
Реализуйте для класса Rational операции сложения и вычитания, а также операции унарного плюса и унарного минуса. Это позволит выполнять над обыкновенными дробями базовые арифметические операции, так же как с целыми числами и числами с плавающей запятой.

Подсказка: 
Возьмите код сложения дробей из теории. Операцию вычитания реализуйте аналогично. Операции унарного плюса и минуса реализуйте по аналогии со структурой Vector2D.
*/





class Rational {
public:
    Rational() = default;

    Rational(int numerator)
        : numerator_(numerator)
        , denominator_(1)
    {
    }

    Rational(int numerator, int denominator)
        : numerator_(numerator)
        , denominator_(denominator)
    {
        Normalize();
    }
    
    int Numerator() const {
        return numerator_;
    }

    int Denominator() const {
        return denominator_;
    }
    
     Rational operator+(Rational left /*, Rational right*/) {
    const int numerator = left.Numerator() * /*right.*/Denominator() 
                  + /*right.*/Numerator() * left.Denominator();
    const int denominator = left.Denominator() * /*right.*/Denominator();

    return {numerator, denominator};
    }


     Rational operator-(Rational left /*, Rational right*/) {
        const int numerator = left.Numerator() * /*right.*/ Denominator() 
                      - /*right.*/Numerator() * left.Denominator();
        const int denominator = left.Denominator() * /*right.*/Denominator();

        return {numerator, denominator};
    }


    // Операция унарного плюса возвращает копию переданного экземпляра
    Rational operator+(/*Rational r*/) {
        return /*r*/  {Numerator() , Denominator()};
    }

    // Операция унарного минуса возвращает вектор с противоположным направлением
    Rational operator-(/*Rational r*/) {
        return {-Numerator() , -Denominator()};
    }

private:
    void Normalize() {
        if (denominator_ < 0) {
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }
        int n = gcd(numerator_, denominator_);
        numerator_ /= n;
        denominator_ /= n;
    }

    int numerator_ = 0;
    int denominator_ = 1;
};

ostream& operator<<(ostream& output, Rational rational) {
    output << rational.Numerator() << '/' << rational.Denominator();
    return output;
}

istream& operator>>(istream& input, Rational& rational) {
    int numerator, denominator;
    char slash;
    input >> numerator >> slash >> denominator;
    rational = Rational{numerator, denominator};
    return input;
}

#include <iostream>
#include <numeric>
#include <string>

using namespace std;
/*
Задание 2
Перегрузите бинарные арифметические операции +, -, *, / для класса Rational на основе реализованных вами присваивающих выражений.

ПОДСКАЗКА
В теории вы узнали, как это сделать на примере структуры Vector2D. Здесь сделайте по аналогии.

Например, задав +=, реализуем операцию сложения буквально в одну строку кода: 

Vector2D operator+(Vector2D left, Vector2D right) {
    // аргумент left принят по значению; можно спокойно модифицировать
    // это значение внутри операции и вернуть в виде результата
    return left += right;
} 
*/

class Rational {
public:
    Rational() = default;

    Rational(int value)
        : numerator_(value)
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
    
    
    // МОЙ КОД НАЧИНАЕТСЯ НИЖЕ 

Rational operator+=(Rational right) {
    // Результат операции сохраняется в текущем экземпляре класса
    numerator_ = numerator_*right.Denominator() + right.Numerator() * denominator_;
    denominator_ = denominator_*right.Denominator();  
    Normalize();
    // return *this позволяет вернуть ссылку на текущий объект
    return *this;
}

Rational operator-=(Rational right) {
    // Результат операции сохраняется в текущем экземпляре класса
    numerator_ = numerator_*right.Denominator() - right.Numerator() * denominator_;
    denominator_ = denominator_*right.Denominator();  
    Normalize();
    // return *this позволяет вернуть ссылку на текущий объект
    return *this;
}

Rational operator*=(Rational right) {
    // Результат операции сохраняется в текущем экземпляре класса
    numerator_ = numerator_* right.Numerator();
    denominator_ = denominator_*right.Denominator();  
    Normalize();
    // return *this позволяет вернуть ссылку на текущий объект
    return *this;
}

Rational operator/=(Rational right) {
    // Результат операции сохраняется в текущем экземпляре класса
    numerator_ = numerator_*right.Denominator();
    denominator_ = denominator_*right.Numerator();  
    Normalize();
    // return *this позволяет вернуть ссылку на текущий объект
    return *this;
}
    
// мой новый код тут 
/*Rational operator+(Rational right) {
    Rational left = { numerator_, denominator_};
    return left += right;;
}*/
    

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
    return output << rational.Numerator() << '/' << rational.Denominator();
}

istream& operator>>(istream& input, Rational& rational) {
    int numerator;
    int denominator;
    char slash;
    if ((input >> numerator) && (input >> slash) && (slash == '/') && (input >> denominator)) {
        rational = Rational{numerator, denominator};
    }
    return input;
}

Rational operator+(Rational left, Rational right) {
    const int numerator = left.Numerator() * right.Denominator() + right.Numerator() * left.Denominator();
    const int denominator = left.Denominator() * right.Denominator();
    return {numerator, denominator};
}

Rational operator-(Rational left, Rational right) {
    const int numerator = left.Numerator() * right.Denominator() - right.Numerator() * left.Denominator();
    const int denominator = left.Denominator() * right.Denominator();
    return {numerator, denominator};
}

Rational operator+(Rational value) {
    return value;
}

Rational operator-(Rational value) {
    return {-value.Numerator(), value.Denominator()};
}







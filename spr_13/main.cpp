#include <iostream>
#include <string>
#include <string_view>
#include <memory>
#include <typeinfo>

using namespace std;

/*
Тестовый 
42
abc
construct
move
destruct
construct
copy
destruct
destruct
destruct


Мой вывод 
42
abc
construct
move
destruct
construct
copy -- Лишнее 
move -- лишнее 
destruct -- лишнее 
destruct
destruct
destruct
*/


class AnyStorageBase {
public:
    virtual ~AnyStorageBase() {}
    virtual void Print(ostream& out) const = 0;
};



template <typename T>
class AnyStorage : public AnyStorageBase {
public:
    template <typename T>
    AnyStorage(T&& value) : data_(std::forward<T>(value)) {}

    void Print(ostream& out) const override {
        out << data_;
    }

private:
    T data_;
};

/*

template <typename T>
class AnyStorage : public AnyStorageBase {
public:
    AnyStorage(T value) : data_(std::move(value)) {}

    void Print(ostream& out) const override {
        out << data_;
    }

private:
    T data_;
};
*/

class Any {
public:
    template <typename S>
    Any(S type)  {
    
        using Initial = std::remove_reference_t<S>;
        ptr_ = std::make_unique<AnyStorage<Initial>>(std::forward<S>(type));
    }

    void Print(ostream& out) const {
        if (ptr_) {
            ptr_->Print(out);
        }
    }

    //~Any() { ptr_ = nullptr; }

private:
    unique_ptr<AnyStorageBase> ptr_;
};

class Dumper {
public:
    Dumper() {
        std::cout << "construct"sv << std::endl;
    }
    ~Dumper() {
        std::cout << "destruct"sv << std::endl;
    }
    Dumper(const Dumper&) {
        std::cout << "copy"sv << std::endl;
    }
    Dumper(Dumper&&) {
        std::cout << "move"sv << std::endl;
    }
    Dumper& operator=(const Dumper&) {
        std::cout << "= copy"sv << std::endl;
        return *this;
    }
    Dumper& operator=(Dumper&&) {
        std::cout << "= move"sv << std::endl;
        return *this;
    }
};

ostream& operator<<(ostream& out, const Any& arg) {
    arg.Print(out);
    return out;
}

ostream& operator<<(ostream& out, const Dumper&) {
    return out;
}

int main() {
    Any any_int(42);
    Any any_string("abc"s);

    // операция вывода Any в поток определена чуть выше в примере
    cout << any_int << endl << any_string << endl;

    Any any_dumper_temp{ Dumper() };

    Dumper auto_dumper;
    Any any_dumper_auto(auto_dumper);
}

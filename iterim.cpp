#include <cassert>
#include <iostream>
#include <string_view>
#include <vector>
#include <algorithm>
#include <map>


using namespace std;

/*
Самый простой способ реализации словаря — это, конечно, контейнер-словарь. В данном случае удобно держать два контейнера, чтобы обеспечить поиск слов и для прямого перевода, и для обратного. Хранить в этих словарях строки — расточительство. Но строки вам всё равно нужны, так как строки могут быть уничтожены до уничтожения объекта Translator. Сохраните их себе в удобном контейнере.
*/


class Translator {
public:
    Translator() {} // констуруктор по умолчани.
    
    void Add(string_view source, string_view target){
            forward_.emplace(std::make_pair(source, target) );
            backward_.emplace(std::make_pair(target, source) );
    }
    
    string_view TranslateForward(string_view source) const {
        string_view res{};
        //map::const_iterator pos = forward_.find("source");
        auto pos = forward_.find(source);
        if (pos != forward_.end()) {
           return pos->second;
            }
        else {return res;}
    }
    
    string_view TranslateBackward(string_view target) const {
        string_view res{};
        //map::const_iterator pos = backward_.find(target);
        auto pos = backward_.find(target);
        if (pos != backward_.end()) {
           return pos->second;
            }
        else {return res;}
    }
        


private:
    // ...
    map<string_view, string_view> forward_;
    map<string_view, string_view> backward_;
};



void TestSimple() {
    Translator translator;
    translator.Add(string("okno"s), string("window"s));
    translator.Add(string("stol"s), string("table"s));

    assert(translator.TranslateForward("okno"s) == "window"s);
    assert(translator.TranslateBackward("table"s) == "stol"s);
    assert(translator.TranslateForward("table"s) == ""s);
}

int main() {
    TestSimple();
    return 0;
}

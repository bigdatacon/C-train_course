#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>

using namespace std;
/*template <typename F>
string BruteForceInternal(F check, string prefix, int length) {
    // Поиск паролей длины на более length начинающихся со строки prefix
    static string symbols("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    if (check(prefix)) {
        return prefix;
    }
    if (prefix.length() < length) {
        for (auto symbol: symbols) {
            string found = BruteForceInternal(check, prefix + symbol, length);
            if (!found.empty()) {
                return found;
            }
        }
    }
    return string();
}


// -- реализация через рекурсию
template <typename F>
string BruteForce(F check) {
    for(int length = 1; length < 1000; length++) {
        string found = BruteForceInternal(check, ""s, length);
        if (!found.empty()) {
            return found;
        }
    }
    return string();
}*/






// -- реализация через цикл 
template <typename F>
string BruteForce(F check) {
    static string symbols("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    vector<string> one;
    vector<string> two;
    vector<string> three;
    vector<string> four;
    vector<string> five;
    
    for (auto symbol: symbols) {
        string s {symbol};
        one.push_back(s);
        if (check(s)){return s;}
    }
    for (auto symbol: symbols) {
        for (auto el : one) {string s = el + symbol; two.push_back(s); }
    }
    for (auto symbol: symbols) {
        for (auto el : two) {string s = el + symbol; three.push_back(s); /*cout << " s : " << s<<  endl; */ }
    }
    
    for (auto el : three) 
    {if (check(el)) { return el;}}*/
    
    /*for (auto symbol: symbols) {
        for (auto el : three) {string s = el + symbol; four.push_back(s); }
    }
    
    for (auto symbol: symbols) {
        for (auto el : four) {string s = el + symbol; five.push_back(s); }
    }
    
    for (auto el : five) 
    {if (check(el)) { return el;}}*/
        
    return string();
}



// -- реализация через рекурсию
/*template <typename F>
string BruteForce(F check) {
    // верните строку str, для которой check(str) будет true
    char one =  'A' + rand() % 26;
    char two =  'A' + rand() % 26;
    char three =  'A' + rand() % 26;
    char four =  'A' + rand() % 26;
    char five =  'A' + rand() % 26;
    string s{one, two, three, four, five};

    if (!check(s)){BruteForce(check);}
    return s;
}*/


// - тут заготовка которую я не правил 
/*template <typename F>
string BruteForce(F check) {
    // верните строку str, для которой check(str) будет true
}*/

int main() {
    string pass = "ARTUR"s;
    auto check = [pass](const string& s) {
        return s == pass;
    };
    cout << BruteForce(check) << endl;
}

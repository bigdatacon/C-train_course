#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

using namespace std;

/*
Читайте слова из потока в переменную string word таким образом: while (text >> word)
Создайте ассоциативный контейнер, который будет хранить слова по хешу
Подойдёт unordered_map<size_t, unordered_set<string>>
Если для хеша есть слово, и вы нашли другое, возникает коллизия
Где можно используйте move для слов
*/

template <typename Hash>
int FindCollisions(const Hash& hasher, istream& text) {
    // место для вашей реализации
    int colision = 0;
    unordered_map<size_t, unordered_set<string>> res;

    string word;
    while (text >> word) {
        size_t hash_string = hasher(word);
        if (!res.count(hash_string)) {
            res[hash_string].insert(move(word));
        }
        else {
            ++colision;
            res[hash_string].insert(move(word));
        }

    }
    return colision;
}

// Это плохой хешер. Его можно использовать для тестирования.
// Подумайте, в чём его недостаток
struct HasherDummy {
    size_t operator() (const string& str) const {
        size_t res = 0;
        for (char c : str) {
            res += static_cast<size_t>(c);
        }
        return res;
    }
};

int main() {
    hash<string> str_hasher;
    int collisions = FindCollisions(str_hasher, cin);
    cout << "Found collisions: "s << collisions << endl;
}

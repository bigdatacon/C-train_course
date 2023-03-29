#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <sstream>

using namespace std;

template <typename Hash>
int FindCollisions(const Hash& hasher, istream& text) {
    int colision = 0;
    unordered_map<size_t, unordered_set<string>> res;

    string word;
    while (text >> word) {
        //if (word.empty()) { break; }
        size_t hash_string = hasher(word);
        res[hash_string].insert(move(word));
        if (res[hash_string].size() !=1) {++colision;}
        
        
        /*if (!res.count(hash_string)) {
            res[hash_string].insert(move(word));
        }
        else {
            ++colision;
        }*/

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

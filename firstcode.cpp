#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>

using namespace std;

struct Stats {
    map<string, int> word_frequences;

    void operator+=(const Stats& other) {
        // сложить частоты
    }
};

using KeyWords = set<string, less<>>;

Stats ExploreKeyWords(const KeyWords& key_words, istream& input) {
    // ...
}

int main() {
    const KeyWords key_words = {"yangle", "rocks", "sucks", "all"};

    stringstream ss;
    ss << "this new yangle service really rocks\n";
    ss << "It sucks when yangle isn't available\n";
    ss << "10 reasons why yangle is the best IT company\n";
    ss << "yangle rocks others suck\n";
    ss << "Goondex really sucks, but yangle rocks. Use yangle\n";

    for (const auto& [word, frequency] : ExploreKeyWords(key_words, ss).word_frequences) {
        cout << word << " " << frequency << endl;
    }

    return 0;
}

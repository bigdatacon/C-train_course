#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <future>

#include <algorithm>
#include <execution>
//#include <list>

using namespace std;

struct Stats {
    map<string, int> word_frequences;

    void operator+=(const Stats& other) {
        for (const auto& [word, frequence] : other.word_frequences) {
            word_frequences[word] += frequence;
        }
    }
};

vector<string_view> SplitIntoWords(string_view text) {
    vector<string_view> result;
    size_t pos = text.find_first_not_of(" ");
    text.remove_prefix(min(text.size(), pos));
    const size_t pos_end = text.npos;

    while (!text.empty()) {
        size_t space = text.find(' ');
        result.push_back(space == pos_end ? text.substr(0, pos_end) : text.substr(0, space));
        pos = text.find_first_not_of(" ", space);
        text.remove_prefix(min(text.size(), pos));
    }
    return result;
}

using KeyWords = set<string, less<>>;

Stats ExploreKeyWordsStr(const KeyWords& key_words, const string& s) {
    Stats result;
    vector<string_view> words = SplitIntoWords(s);

    for (string_view word : words) {
        if (key_words.count(word)) {
            result.word_frequences[std::string(word)] += 1;
        }
    }
    return result;
}

Stats ExploreKeyWords(const KeyWords& key_words, istream& input) {
    Stats result;
    vector<future<Stats>> processes;

    for (string s; getline(input, s); ) {
        processes.push_back(async(ExploreKeyWordsStr, cref(key_words), s));
        if (processes.size() >= 5000 || (input.peek() == EOF || input.eof())) {
            for_each(execution::par, processes.begin(), processes.end(), //Вариант 2
                [&result](auto& item) {result += item.get(); });
             //return result;

        }
    }
    return result;
}

int main() {
    const KeyWords key_words = { "yangle", "rocks", "sucks", "all" };

    stringstream ss;
    ss << "this new yangle service really rocks\n";
    ss << "It sucks when yangle isn't available\n";
    ss << "10 reasons why yangle is the best IT company\n";
    ss << "yangle rocks others suck\n";
    ss << "Goondex really sucks, but yangle rocks. Use yangle\n";

    for (const auto& [word, frequency] : ExploreKeyWords(key_words, ss).word_frequences) {
        cout << word << " " << frequency << endl;
    }
    // rocks - 2, sucks - 1, yangle - 6

    return 0;
}

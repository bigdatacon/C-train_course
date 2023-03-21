#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <iostream>
#include <sstream>

using std::istringstream;
using std::string;
using std::cout;

using namespace std;


#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <execution>
#include <future>
#include <algorithm>

using namespace std;

/* Подсказка Разные строки текста или наборы этих строк можно обрабатывать параллельно, а затем складывать полученные словари.
Если собираетесь добавлять future в вектор, имейте в виду: это некопируемый тип.*/

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            if (isprint(c)) {
                word += c;
            }
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}


struct Stats {
    map<string, int> word_frequences;

    void operator+=(const Stats& other) {
        for (const auto& [word, frequence] : other.word_frequences) {
            word_frequences[word] += frequence;
        }
    }
};

using KeyWords = set<string, less<>>;

std::string safe_getline(istream& input) {
    std::string line;
    getline(input, line);
    return move(line);
}

/* Подсказка Разные строки текста или наборы этих строк можно обрабатывать параллельно, а затем складывать полученные словари.
Если собираетесь добавлять future в вектор, имейте в виду: это некопируемый тип.*/
Stats ExploreKeyWords(const KeyWords& key_words, istream& input) {

    Stats stat;
    vector<future<Stats>> all_words;
    std::string line;
    while  (!(line = safe_getline(input)).empty())

    { // читаем строку из входного потока, пока не достигнем конца
        //std::string line = safe_getline(input);
        cout << "line before : " << line << endl;
       
        all_words.push_back(async(SplitIntoWords, line));
    }

    
    for_each(execution::par, all_words.begin(), all_words.end(), 
        [&stat](auto& item) {stat += item.get(); });

    return stat;

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

    return 0;
}

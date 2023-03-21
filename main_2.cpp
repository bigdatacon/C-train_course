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
        // сложить частоты
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
    vector<string> all_words;
    std::string line;
    //std::istream& input = std::cin; // можно использовать любой 
    while  (!(line = safe_getline(input)).empty())

    { // читаем строку из входного потока, пока не достигнем конца
        //std::string line = safe_getline(input);
        cout << "line before : " << line << endl;
       
        auto async_ = async(SplitIntoWords, line);
        //all_words.push_back(async(SplitIntoWords, line));
        all_words.insert(all_words.end(), async_.get().begin(), async_.get().end());
        
  

       /* vector<string> words = SplitIntoWords(line);
        cout << "words.size() : " << words.size() << endl;
        //all_words.insert(all_words.end(), words.begin(), words.begin() );
        all_words.insert(all_words.end(), words.begin(), words.end());

        words.clear();*/
    }

    for (auto word : all_words) {
        if (key_words.count(word)) {
            int freq = count(all_words.begin(), all_words.end(), word);
            stat.word_frequences[word] = freq;
        }
    }

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

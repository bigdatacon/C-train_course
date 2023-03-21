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
// тут пример разбиения строк на слова через issstream 
/*int main() {
    std::vector<std::string> my_vector{ "Hello, world!", "How are you?", "Nice to meet you." };

    std::vector<std::vector<std::string>> words_vector;

    std::transform(my_vector.begin(), my_vector.end(), std::back_inserter(words_vector), [](std::string s) {
    std::vector<std::string> words;
    std::string word;
    std::istringstream stream(s); // превращаем строку s в поток
    while (stream >> word) { // парсим слова из потока
        words.push_back(word);
    }
    return words;
        });
    

    for (auto el : words_vector) { for (auto e : el) { cout << e << endl; } }
    // выводим каждое слово вместе с номером строки
    for (int i = 0; i < words_vector.size(); ++i) {
        for (int j = 0; j < words_vector[i].size(); ++j) {
            std::cout << "Строка " << i << ", слово " << j << ": " << words_vector[i][j] << std::endl;
        }
    }

    return 0;
}*/

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
            if (isprint(c) ) {
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
/* Подсказка Разные строки текста или наборы этих строк можно обрабатывать параллельно, а затем складывать полученные словари.
Если собираетесь добавлять future в вектор, имейте в виду: это некопируемый тип.*/
Stats ExploreKeyWords(const KeyWords& key_words, istream& input) {

    Stats stat;
    vector<string> all_words;
    std::string line;
    //std::istream& input = std::cin; // можно использовать любой 
    while (std::getline(input, line)) { // читаем строку из входного потока, пока не достигнем конца
        cout << "line before : " << line << endl;
        //line.erase(std::remove(line.begin(), line.end(), '"'));
        //cout << "line after : " << line << endl;
        
        /*for_each(line.begin(), line.end(), [&all_words](auto& s) {
            auto async_ = async([&s] { return SplitIntoWords(*s); });
        all_words.insert(async_.get().end(), async_.get().begin(), all_words.end());
            });

        // вариант без async - дублирует то что выше, но тоже не работает
        for (auto el : line) {
            vector<string> words = SplitIntoWords(el);
            all_words.insert(words.end(), words.begin(), all_words.end());

            words.clear();
        }*/

        vector<string> words = SplitIntoWords(line);
        cout << "words.size() : " << words.size() << endl;
        all_words.insert(all_words.end(), words.begin(), words.begin() );

        words.clear();
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

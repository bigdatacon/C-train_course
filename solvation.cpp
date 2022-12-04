#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;
const int MAX_RESULT_DOCUMENT_COUNT = 5;

/*
Замените функцию AddDocument на класс SearchServer из урока. Функции SplitIntoWords и SplitIntoWordsNoStop оставьте, а остальной код можете закомментировать.
*/

/*
Перенесите функцию SplitIntoWordsNoStop внутрь класса, в самый его конец: под private: и поля. Оставьте у полученного метода один параметр — строку, а множество стоп-слов в теле метода получите из поля класса, как это уже сделано в AddDocument. Тогда при вызове SplitIntoWordsNoStop не нужно будет передавать стоп-слова.
*/

/*
Добавьте в SearchServer приватное поле stop_words_ типа set<string>. В этом поле будут храниться стоп-слова поисковой системы.
Перенесите функцию ParseStopWords внутрь класса (под public:!) и переименуйте в SetStopWords. Полученные стоп-слова добавляйте сразу во множество stop_words_. Для этого используйте метод insert.
*/


vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

vector<string> SplitIntoWordsNoStop(const string& text, const set<string>& stop_words) {
    vector<string> words;
    for (const string& word : SplitIntoWords(text)) {
        if (stop_words.count(word) == 0) {
            words.push_back(word);
        }
    }
    return words;
}



class SearchServer {
public:
    void AddDocument(int document_id, const string& document) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        documents_.push_back({document_id, words});
    }
private:
    struct DocumentContent {
        int id = 0;
        vector<string> words;
    };
    vector<DocumentContent> documents_;
    set<string> stop_words_;
    
    vector<string> SplitIntoWordsNoStop(const string& text) {
    vector<string> words;
    for (const string& word : SplitIntoWords(text)) {
        if (stop_words_.count(word) == 0) {
            words.push_back(word);
        }
    }
    return words;
    }
    
    set<string> SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
        return stop_words_;
    }
    
};

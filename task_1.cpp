#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;
const int MAX_RESULT_DOCUMENT_COUNT = 5;

/*
Функция CreateSearchServer должна иметь пустой список параметров и возвращать SearchServer.
В первую очередь создайте объект для заполнения: SearchServer search_server;.
Затем считайте и задайте стоп-слова: search_server.SetStopWords(/* тут строка из ввода */
/*);.
И, наконец, считайте количество документов и в цикле с соответствующим числом итераций вызывайте метод search_server.AddDocument, передавая id документа и его содержимое.
*/

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}
 
int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

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
    
    set<string> SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
        return stop_words_;
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
    

    
};


// считывает из cin стоп-слова и документ и возвращает настроенный экземпляр поисковой системы
SearchServer CreateSearchServer() {
    SearchServer search_server; // 1 create class object 
    
    //2 Затем считайте и задайте стоп-слова: search_server.SetStopWords(/* тут строка из ввода */);*/.
    
    const string stop_words_joined = ReadLine();
    //const set<string> stop_words = ParseStopWords(stop_words_joined);  -- так было до класса 
    search_server.SetStopWords(stop_words_joined);
    
    /*3 И, наконец, считайте количество документов и в цикле с соответствующим числом итераций вызывайте метод search_server.AddDocument, передавая id документа и его содержимое.*/
    
    //vector<pair<int, vector<string>>> documents;
    const int document_count = ReadLineWithNumber();
    
    //Правильное представение ADD DOCUMENT   
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument( document_id, ReadLine());
    }

    return search_server;
}

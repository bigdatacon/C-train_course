#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;
const int MAX_RESULT_DOCUMENT_COUNT = 5;

/*
Перенесите FindAllDocuments в приватную часть класса, оставьте только один параметр — множество query_words. Переделать нужно все участки кода, в которых используется массив documents, множество stop_words и функция (теперь уже метод) SplitIntoWordsNoStop.
Перенесите FindTopDocuments в публичную часть класса и проделайте те же преобразования.
Перенесите функцию MatchDocument в приватную часть класса, сделав её статическим методом.ъ
Функцию ParseQuery тоже нужно сделать методом класса.
В функции main вызовите CreateSearchServer и положите результат в переменную (пока не константную!). Затем вызовите метод FindTopDocuments и запустите цикл по его результату с выводом ответа.
Все используемые в методах класса сущности — константы, типы, функции — объявите до кода класса.
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

/*
1 Перенесите FindAllDocuments в приватную часть класса, оставьте только один параметр — множество query_words. 
????? 
Переделать нужно все участки кода, в которых используется массив documents, множество stop_words и функция (теперь уже метод) SplitIntoWordsNoStop.

3 Перенесите FindTopDocuments в публичную часть класса и проделайте те же преобразования.

4 Перенесите функцию MatchDocument в приватную часть класса, сделав её статическим методом.ъ

5 Функцию ParseQuery тоже нужно сделать методом класса.


В функции main вызовите CreateSearchServer и положите результат в переменную (пока не константную!). Затем вызовите метод FindTopDocuments и запустите цикл по его результату с выводом ответа.
Все используемые в методах класса сущности — константы, типы, функции — объявите до кода класса.
*/

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
    
    vector<Document> FindTopDocuments(/*const vector<DocumentContent>& documents,
                                  const set<string>& stop_words,*/ const string& raw_query) {
    const set<string> query_words = ParseQuery(raw_query, stop_words);
    auto matched_documents = FindAllDocuments(documents, query_words);
 
    sort(matched_documents.begin(), matched_documents.end(), HasDocumentGreaterRelevance);
    if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
        matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
    }
    return matched_documents;
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
    
    vector<Document> FindAllDocuments(/*const vector<DocumentContent>& documents,*/ const set<string>& query_words)
    {vector<Document> matched_documents;
    for (const auto& document : documents) {
        const int relevance = MatchDocument(document, query_words);
        if (relevance > 0) {
            matched_documents.push_back({document.id, relevance});
        }
    }
    return matched_documents;
    }
    
    static int MatchDocument(const DocumentContent& content, const set<string>& query_words) {
    if (query_words.empty()) {
        return 0;
    }
    set<string> matched_words;
    for (const string& word : content.words) {
        if (matched_words.count(word) != 0) {
            continue;
        }
        if (query_words.count(word) != 0) {
            matched_words.insert(word);
        }
    }
    return static_cast<int>(matched_words.size());
    }
    
    set<string> ParseQuery(const string& text, const set<string>& stop_words) {
    set<string> query_words;
    for (const string& word : SplitIntoWordsNoStop(text, stop_words)) {
        query_words.insert(word);
    }
    return query_words;
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

int main() {
    
    /*В функции main вызовите CreateSearchServer и положите результат в переменную (пока не константную!). Затем вызовите метод FindTopDocuments и запустите цикл по его результату с выводом ответа.
Все используемые в методах класса сущности — константы, типы, функции — объявите до кода класса.*/
    
    SearchServer result = CreateSearchServer();
    for (auto [document_id, relevance] : result.FindTopDocuments()) {
        cout << "{ document_id = "s << document_id << ", relevance = "s << relevance << " }"s
            << endl;
    }
    
    
}


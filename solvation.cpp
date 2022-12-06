#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

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

struct Document {
    int id;
    int relevance;
};

struct DocumentQuery {
    set<string> minuswords;
    set<string> pluswords;
};


class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        documents_.push_back({document_id, words});
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const DocumentQuery query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words.pluswords, query_words.minuswords);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 return lhs.relevance > rhs.relevance;
             });
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

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    DocumentQuery ParseQuery(const string& text) const {
        set<string> pluswords;
        set<string> minuswords;

        DocumentQuery query_words;
        
        for (const string& word : SplitIntoWordsNoStop(text)) {
            cout << "here query w " << word << endl;
            char ch = word.at(0);
            if (ch == '-') {cout << "here query w word.at(0)):  " << int(ch)  << endl;
                           minuswords.insert(word.substr(1));}
            else {
            pluswords.insert(word);}
        }
        query_words.minuswords = minuswords; 
        query_words.pluswords = pluswords;
        for (auto el : query_words.minuswords){cout << "here stop word:  " << el  << endl;}
        
        
        return query_words;
    }
    
    bool CheckMinusWords(const DocumentContent& content, const set<string>& minus_words){
        for (const auto& word : content.words) {
            if (minus_words.count(word)){return true;}
            else {return false;}
        }
    }
    
    vector<Document> FindAllDocuments(const set<string>& query_words, const set<string>& minus_words) const {

        vector<Document> matched_documents;
        for (const auto& document : documents_) {
            
            if (CheckMinusWords(document,  query_words)){continue;}
            else {
            
            const int relevance = MatchDocument(document, query_words);
            if (relevance > 0) {
                matched_documents.push_back({document.id, relevance});
            }
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
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }

    return search_server;
}

/*Минус-слова запроса должны обрабатываться до плюс-слов. Каждый документ, где есть минус-слово, не должен включаться в результаты поиска, даже если в нём присутствуют плюс-слова.
Для хранения запроса удобно создать структуру Query с двумя множествами слов: плюс- и минус-словами. Возвращать эту структуру по строке запроса нужно в новом приватном методе — ParseQuery.
После сравнения первого символа с '-' не забудьте отрезать этот минус вызовом .substr(1), а затем проверить результат по словарю стоп-слов.*/




int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s << endl;
    }
}

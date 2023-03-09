#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <cmath>

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
    double relevance;
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
        if (!words.empty()){

            double dolya_w = 1.0/words.size();
            ++count_doc_;
            if (words.size()!=0) {
                for (const auto& word :  words){
                    word_to_document_freqs_[word][document_id]+=dolya_w;
                }
            }
        }
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
    int count_doc_ = 0;
    map<string, map<int, double>> word_to_document_freqs_;
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
            char ch = word.at(0);
            if (ch == '-'){
                minuswords.insert(word.substr(1));
            } else {
                pluswords.insert(word);
            }
        }
        query_words.minuswords = minuswords;
        query_words.pluswords = pluswords;
        return query_words;
    }
    
    double GetIdf(const string& str) const {
        return log((count_doc_*1.0)/word_to_document_freqs_.at(str).size());
    } 
    
    vector<Document> FindAllDocuments(const set<string>& query_words, const set<string>& minus_words) const {
        map<int, double> document_to_relevance;
        vector<Document> matched_documents;
        vector<int> numbers_to_delete;
        
        if (!query_words.empty()){
            for (auto str : query_words){
                if  (word_to_document_freqs_.find(str) != word_to_document_freqs_.end()) {
                    //double word_idf = log((count_doc_*1.0)/word_to_document_freqs_.at(str).size());
                    double word_idf = GetIdf(str);
                    
                    for (const auto& [doc_id, word_tf] : word_to_document_freqs_.at(str) ){
                        double tf_idf = word_idf*word_tf;
                        document_to_relevance[doc_id] += tf_idf;
                    }
                }
                continue;
            }
            

            for ( auto minus_word : minus_words) {
                if  (word_to_document_freqs_.find(minus_word) != word_to_document_freqs_.end()) {
                    for (const auto& [doc_id, word_tf] : word_to_document_freqs_.at(minus_word)) {
                        numbers_to_delete.push_back(doc_id);
                    }
                }
            }

            for (const int& number : numbers_to_delete) {
                document_to_relevance.erase(number);
            }
            
            for (auto el : document_to_relevance){
                matched_documents.push_back({el.first, el.second});
            }
            
        }
        return matched_documents;
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


int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s << endl;
    }
}

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
        //int count_doc_ = 0;
        //map<string, map<int, double>> word_to_document_freqs_;
        double dolya_w = 1.0/words.size(); 
        ++count_doc_;
        if (words.size()!=0) {
        for (/*const*/ auto word :  words){
            //word_to_documents_[word].insert(document_id);
            word_to_document_freqs_[word][document_id]+=dolya_w;}
            
                           }
        
                //добавляю в word_idf_ : словарь  слово : IDF 
        /*if (word_to_documents_.size()!=0){
        for (const auto& [key, value]: word_to_documents_) {

        //word_idf_.insert({key, log(double(count_doc_)/value.size())});
        word_idf_[key] = log(double(count_doc_)/value.size());
        }} */
        
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
    //map<string, double> word_idf_; // словарь : слово IDF
    int count_doc_ = 0;
    map<string, map<int, double>> word_to_document_freqs_;
    //map<string, set<int>> word_to_documents_;

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
            //cout << "here query w " << word << endl;
            char ch = word.at(0);
            if (ch == '-') /*(word[0] == '-')*/ {/*cout << "here query w word.at(0)):  " << int(ch)  << endl;*/
                           minuswords.insert(word.substr(1));}
            else {
            pluswords.insert(word);}
        }
        query_words.minuswords = minuswords; 
        query_words.pluswords = pluswords;

        
        
        return query_words;
    }
    
  
    // функция проверки что хотя бы 1 слово из запроса есть в докуентах
        /*bool CheckAnyWordsinDoc(const set<string>& query_words) const {
  
           for (auto str : query_words){ 
                    for (auto word_to_doc : word_to_documents_ ){
                    if (word_to_doc.first ==str){ return true;}
                                                                        }
                                       }
                                                                
            return false; 
}*/

    
    vector<Document> FindAllDocuments(const set<string>& query_words, const set<string>& minus_words) const {
        map<int, double> document_to_relevance;
        vector<Document> matched_documents;
        vector<int> numbers_to_delete;     //вектор с номерами id для удаления
        
        if (!query_words.empty()){
        //word_to_document_freqs_
        //count_doc_
        //map<string, set<int>> word_to_documents_
        //map<string, double> word_idf_; // словарь : слово IDF
        
        for (auto str : query_words){ 
            //double word_idf = word_idf_.at(str); // получаю IDF слова 
            double word_idf = log((count_doc_*1.0)/word_to_document_freqs_.at(str).size());
            for (const auto& [doc_id, word_tf] : word_to_document_freqs_.at(str) ){
            double tf_idf = word_idf*word_tf;
            document_to_relevance[doc_id] += tf_idf;
                                        } }
            
        
        for ( auto minus_word : minus_words) {
        for (const auto& [doc_id, word_tf] : word_to_document_freqs_.at(minus_word)) {
        numbers_to_delete.push_back(doc_id);   //вектор с номерами id для удаления
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

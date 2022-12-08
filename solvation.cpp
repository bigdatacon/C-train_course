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

    void AddDocument(int document_id, const string& document, const int document_count) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        //map<string, set<int>> word_to_documents_;
        //map<string, double> word_idf_;


        if (words.size()!=0) {
        for (/*const*/ auto word :  words){word_to_documents_[word].insert(document_id);}
                           }
        //добавляю в word_idf_ : словл : IDF 
        for (const auto& [key, value]: word_to_documents_) {
        word_idf_.insert({key, log(document_count/value.size())}); 
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
    map<string, double> word_idf_;
    map<string, map<int, double>> word_to_document_freqs_;
    map<string, set<int>> word_to_documents_;

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
            if (ch == '-') {/*cout << "here query w word.at(0)):  " << int(ch)  << endl;*/
                           minuswords.insert(word.substr(1));}
            else {
            pluswords.insert(word);}
        }
        query_words.minuswords = minuswords; 
        query_words.pluswords = pluswords;

        
        
        return query_words;
    }
    
   /* bool CheckMinusWords(const DocumentContent& content, const set<string>& minus_words) const {
        if (content.words.empty()) {
            return false;
        }         
        for (const auto& word : content.words) {
               if (minus_words.count(word)){ return true;}
        }
        return false;
    }*/
  
    // функция проверки что хотя бы 1 слово из запроса есть в докуентах
        bool CheckAnyWordsinDoc(const set<string>& query_words) const {
  
           for (auto str : query_words){ 
                    for (auto word_to_doc : word_to_documents_ ){
                    if (word_to_doc.first ==str){ return true;}
                                                                        }
                                       }
                                                                
            return false; 
}
    /*Как и раньше, для поиска документов и вычисления их релевантности хватит двойной вложенности циклов for: внешний — по словам запроса, внутренний — по документам, где это слово встречается.
Помимо добавления количества документов, достаточно будет переделать словарь «слово → документы» в более сложную структуру map<string, map<int, double>> word_to_document_freqs_, которая сопоставляет каждому слову словарь «документ → TF». Подумайте, как с этими данными быстро вычислять IDF.
Для хранения релевантности найденных документов используйте контейнер map<int, double>. Ключи в нём — id найденных документов, а значения вычисляются по формуле TF-IDF, описанной в уроке.*/
    vector<Document> FindAllDocuments(const set<string>& query_words, const set<string>& minus_words) const {
        map<int, int> document_to_relevance;
        vector<Document> matched_documents;
        map<string, map<int, double>> word_to_document_freqs_;
        
        if (CheckAnyWordsinDoc(query_words)){
        
        for (auto str : query_words){ 
            for (auto word_to_doc : word_to_documents_ ){
            if (word_to_doc.first ==str){
            set <int> doc_ids =word_to_doc.second;
            for (auto id : doc_ids) {++document_to_relevance[id];
                                    }
                                                         }
                                        } 
                                    }
        for (auto str : minus_words){ 
            for (auto word_to_doc : word_to_documents_ ){
            if (word_to_doc.first ==str){
            set <int> doc_ids =word_to_doc.second;
            for (auto id : doc_ids) {/*--document_to_relevance[id]; */
                std::map<int,int>::iterator it;
                it = document_to_relevance.find(id);
                if (it!=document_to_relevance.end()){
                                      document_to_relevance.erase (it);};
                                    }
                                                         }
                                        } 
                                    }
        for (auto el : document_to_relevance){
            matched_documents.push_back({el.first, el.second});
                                             }             
            
                                                                    }
        return matched_documents;
                    }
                                                                                                     


    /*static int MatchDocument(const DocumentContent& content, const set<string>& query_words) {
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
    }*/
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());

    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine(), document_count);
    }

    return search_server;
}

// IDF  =log(количество документов / количество документов) где слово встречается. Рассчитывается для слова 

// TF  = количество раз сколько слово встречается в документе / на количество слов в документе. Рассчитывается для каждого слова и докумена 

// TF-IDF: считается для каждого документа и строки запроса. Для этого IDF каждого слова запроса * TF каждого слова в запроса  и далее суммируются ти произведения. Так получается TF IDF документа.

int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s << endl;
    }
}

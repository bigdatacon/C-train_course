#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <map>

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

/*Вместо vector<DocumentContent> documents_ объявите поле map<string, set<int>> word_to_documents_. В нём будет храниться инвертированный индекс документов. Структуру DocumentContent удалите.*/


class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }
    /*2. В методе AddDocument переберите в цикле все слова документа, кроме стоп-слов. Вставьте в множество документов, соответствующих очередному слову документа, id вставляемого документа. Так очередной документ будет добавлен в инвертированный индекс.*/
    void AddDocument(int document_id, const string& document) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        //map<string, set<int>> word_to_documents_;
        for (/*const*/ auto word :  words){word_to_documents_[word].insert(document_id);}
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
    
    /*3. В методе FindAllDocuments объявите переменную document_to_relevance типа map<int, int>. В ней ключ — id найденного документа, а значение — релевантность соответствующего документа. Она равна количеству плюс-слов, найденных в нём.*/
    /*4. В методе FindAllDocuments переберите в цикле все плюс-слова поискового запроса. Если в word_to_documents_ есть плюс-слово, увеличьте в document_to_relevance релевантности всех документов, где это слово найдено. Так вы соберёте все документы, которые содержат плюс-слова запроса.*/
    /*5. Исключите из результатов поиска все документы, в которых есть минус-слова. В методе FindAllDocuments переберите в цикле все минус-слова поискового запроса. Если в word_to_documents_ есть минус-слово, удалите из document_to_relevance все документы с этим минус-словом. Так в document_to_relevance останутся только подходящие документы.*/
    /*6. Перенесите id и релевантности документов из document_to_relevance в vector<Document> и верните результирующий вектор.*/
    
    vector<Document> FindAllDocuments(const set<string>& query_words, const set<string>& minus_words) const {
        map<int, int> document_to_relevance;
        vector<Document> matched_documents;
        
        for (auto str : query_words){ (if word_to_documents_.first ==str) ) {
            set <string> doc_ids =word_to_documents_.second;
            for (auto id : doc_ids) {++document_to_relevance[id];}
        } 
        for (auto str : minus_words){ (if word_to_documents_.first ==str) ) {
            set <string> doc_ids =word_to_documents_.second;
            for (auto id : doc_ids) {--document_to_relevance[id];
                                   // пробую удалить через erase 
                                         /*it = document_to_relevance.find (id);            
                                         document_to_relevance.erase (it);*/
                                    }
        } 
        for (auto el : document_to_relevance){
            Document matched_document;
            matched_document.id  = el.first;
            matched_document.relevance  = el.second;
            matched_documents.push_back(matched_document);
        }             
            return matched_documents;


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




int main() {
    const SearchServer search_server = CreateSearchServer();

    const string query = ReadLine();
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s << endl;
    }
}

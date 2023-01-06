/* Решение из урока №3 (на основе optional) */
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <iterator>
#include <optional>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

/*НЕ СДЕЛАНО */

/* 100       bool b = ChekTwoMinusorEmptyWord(document); 
Стоит использовать явные названия для переменных.
Можно не проверять документы на два минуса. */



/*120             throw invalid_argument("invalid_argument"); 
Не стоит дублировать проверку перед каждым вызовом ParseQuery */

/*168         bool b = ChekTwoMinusorEmptyWord(raw_query); 
Не стоит дублировать проверку перед каждым вызовом ParseQuery */

/*249         if (ch == '-') { return false; }  // проверка что строка не оканчивается на "-" 
такие проверки лучше производить с готовыми словами в ParseQueryWord*/   // ????Как это сделать если ParseQueryWord возвращает структура и там чтобы bool вернуть нужно много правок вносить и использовать 
//optional что сложно




/*СДЕЛАНО */
/*104 if (document_id < 0 || count(docs_ids_.begin(), docs_ids_.end(), document_id) != 0 || IsValidWord(document) == false) 
можно не перебирать все индексы 
в documents_.  все индексы хранятся как ключи.*/
/*157             if (find(docs_ids_.begin(), docs_ids_.end(), index) != docs_ids_.end()) 
Не нужно проверять, что есть такой индекс, достаточно просто вернуть  docs_ids_.at(index); */
/*247         if (IsValidWord(text) == false) return false; 
if (!IsValidWord(text)) return false; */
/*252             if (text[i] == '-' && (text[i + 1] == '-' || text[i + 1] == ' ')) return false; 
Стоит проверять что двух минусов нет только в начале слова.*/



string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result;
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
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    Document() = default;

    Document(int id, double relevance, int rating)
        : id(id)
        , relevance(relevance)
        , rating(rating) {
    }

    int id = 0;
    double relevance = 0.0;
    int rating = 0;
};

template <typename StringContainer>
set<string> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    set<string> non_empty_strings;
    for (const string& str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};

class SearchServer {
public:
    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stop_words)
        : stop_words_(MakeUniqueNonEmptyStrings(stop_words)) {
        for (auto i : stop_words_) { if (IsValidWord(i) == false) throw invalid_argument("invalid_argument"); }
    }

    explicit SearchServer(const string& stop_words_text)
        : SearchServer(
            SplitIntoWords(stop_words_text))  // Invoke delegating constructor from string container
    {
    }

    inline static constexpr int INVALID_DOCUMENT_ID = -1;

    void AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings) {
        bool b = ChekTwoMinusorEmptyWord(document);
        if (!b)
            throw invalid_argument("invalid_argument");

        /*if (document_id < 0 || count(docs_ids_.begin(), docs_ids_.end(), document_id) != 0 || IsValidWord(document) == false)
            throw invalid_argument("invalid_argument");*/

        if (document_id < 0 || documents_.count(document_id) > 0 || !IsValidWord(document))
            throw invalid_argument("invalid_argument");

        const vector<string> words = SplitIntoWordsNoStop(document);
        const double inv_word_count = 1.0 / words.size();
        for (const string& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
        documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings), status });
        docs_ids_.push_back(document_id);

    }
    template <typename DocumentPredicate>
    vector<Document> FindTopDocuments(const string& raw_query, DocumentPredicate document_predicate) const {
        bool b = ChekTwoMinusorEmptyWord(raw_query);
        if (!b)
            throw invalid_argument("invalid_argument");


        const Query query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query, document_predicate);

        sort(matched_documents.begin(), matched_documents.end(),
            [](const Document& lhs, const Document& rhs) {
                if (abs(lhs.relevance - rhs.relevance) < 1e-6) {
                    return lhs.rating > rhs.rating;
                }
                else {
                    return lhs.relevance > rhs.relevance;
                }
            });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }
    vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus status) const {
        return FindTopDocuments(
            raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
                return document_status == status;
            });
    }
    vector<Document> FindTopDocuments(const string& raw_query) const {
        return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
    }

    int GetDocumentCount() const {
        return documents_.size();
    }

    int GetDocumentId(int index) const {
        /*Не нужно проверять, что есть такой индекс, достаточно просто вернуть  docs_ids_.at(index); */
        if (index < 0 || index >= GetDocumentCount()) { throw out_of_range("index out_of_range"); }
        return docs_ids_.at(index);

        /*
        if (index < 0 || index >= GetDocumentCount()) { throw out_of_range("index out_of_range"); }
        else {
            if (find(docs_ids_.begin(), docs_ids_.end(), index) != docs_ids_.end())
            {
                return docs_ids_.at(index);
            }
             {
                throw out_of_range("index out_of_range");
            }
        }*/

    }
    tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query, int document_id) const {
        bool b = ChekTwoMinusorEmptyWord(raw_query);
        if (!b)
            throw invalid_argument("invalid_argument");
        const Query query = ParseQuery(raw_query);
        vector<string> matched_words;
        for (const string& word : query.plus_words) {

            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.push_back(word);
            }
        }
        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.clear();
                break;
            }
        }
        return make_tuple(matched_words, documents_.at(document_id).status);
    }

private:
    vector<int> docs_ids_;
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };
    const set<string> stop_words_;
    map<string, map<int, double>> word_to_document_freqs_;
    map<int, DocumentData> documents_;

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

    static int ComputeAverageRating(const vector<int>& ratings) {
        if (ratings.empty()) {
            return 0;
        }
        int rating_sum = 0;
        for (const int rating : ratings) {
            rating_sum += rating;
        }
        return rating_sum / static_cast<int>(ratings.size());
    }

    struct QueryWord {
        string data;
        bool is_minus;
        bool is_stop;
    };

    QueryWord ParseQueryWord(string text) const {
        bool is_minus = false;
        // Word shouldn't be empty
        if (text[0] == '-') {
            is_minus = true;
            text = text.substr(1);
        }
        return { text, is_minus, IsStopWord(text) };
    }

    bool ChekTwoMinusorEmptyWord(string text) const {
        if (text.empty()) return false;
        /*Замечание if (!IsValidWord(text)) return false; */
        if (!IsValidWord(text)) return false;
        //if (IsValidWord(text) == false) return false;

        /*Замечание : такие проверки лучше производить с готовыми словами в ParseQueryWord -- если принять это замечание то проверка не "- " в конце слова не будет производиться в AddDocument*/

        char ch = text.back();
        if (ch == '-') { return false; }  // проверка что строка не оканчивается на "-"

        /*Замечание : Стоит проверять что двух минусов нет только в начале слова.*/
        if (text[0] == '-' && (text[1] == '-'))  return false;

        for (int i = 0; i < text.size() - 1; ++i)
        {
            if (text[i] == '-' && (text[i + 1] == '-' || text[i + 1] == ' ') ) return false;
        }

        /* {
            if (text[i + 1] == '-' || text[i + 1] == ' ')  return false;
        }*/

        return true;
    }

    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };

    Query ParseQuery(const string& text) const {
        Query query;
        for (const string& word : SplitIntoWords(text)) {
            const QueryWord query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    query.minus_words.insert(query_word.data);
                }
                else {
                    query.plus_words.insert(query_word.data);
                }
            }
        }
        return query;
    }

    // Existence required
    double ComputeWordInverseDocumentFreq(const string& word) const {
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
    }

    template <typename DocumentPredicate>
    vector<Document> FindAllDocuments(const Query& query,
        DocumentPredicate document_predicate) const {
        map<int, double> document_to_relevance;
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) {
                const auto& document_data = documents_.at(document_id);
                if (document_predicate(document_id, document_data.status, document_data.rating)) {
                    document_to_relevance[document_id] += term_freq * inverse_document_freq;
                }
            }
        }

        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        vector<Document> matched_documents;
        for (const auto [document_id, relevance] : document_to_relevance) {
            matched_documents.push_back(
                { document_id, relevance, documents_.at(document_id).rating });
        }
        return matched_documents;
    }

    static bool IsValidWord(const string& word) {
        // A valid word must not contain special characters
        return none_of(word.begin(), word.end(), [](char c) {
            return c >= '\0' && c < ' ';
            });
    }

};

void PrintDocument(const Document& document) {
    cout << "{ "s
        << "document_id = "s << document.id << ", "s
        << "relevance = "s << document.relevance << ", "s
        << "rating = "s << document.rating
        << " }"s << endl;
}
int main() {
    SearchServer search_server("и в на"s);
    search_server.AddDocument(0, "белый кот и модный ошейник"s, DocumentStatus::ACTUAL, { 8, -3 });
    search_server.AddDocument(1, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
    search_server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });
    search_server.AddDocument(3, "ухоженный скворец евгений"s, DocumentStatus::BANNED, { 9 });
    cout << "ACTUAL by default:"s << endl;
    for (const Document& document : search_server.FindTopDocuments("пушистый ухоженный кот"s)) {
        PrintDocument(document);
    }
    cout << "ACTUAL:"s << endl;
    for (const Document& document : search_server.FindTopDocuments("пушистый ухоженный кот"s, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::ACTUAL; })) {
        PrintDocument(document);
    }
    cout << "Even ids:"s << endl;
    for (const Document& document : search_server.FindTopDocuments("пушистый ухоженный кот"s, [](int document_id, DocumentStatus status, int rating) { return document_id % 2 == 0; })) {
        PrintDocument(document);
    }
    return 0;
}

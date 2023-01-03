#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <iterator>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

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
    }

    explicit SearchServer(const string& stop_words_text)
        : SearchServer(
            SplitIntoWords(stop_words_text))  // Invoke delegating constructor from string container
    {
    }

    // новая реализация

    // Defines an invalid document id
    // You can refer to this constant as SearchServer::INVALID_DOCUMENT_ID
    inline static constexpr int INVALID_DOCUMENT_ID = -1;


    [[nodiscard]] bool AddDocument(int document_id, const string& document, DocumentStatus status,
        const vector<int>& ratings) {
        //4.  Попытка добавить документ с отрицательным id.
        if (document_id < 0) { return false; }
       // 6. Попытка добавить документ с id, совпадающим с id документа, который добавился ранее.
        if (documents_.count(document_id) > 0) { return false; }
        //if (documents_.at(document_id)) { return false; }   - дает ошибку не пойму почему
        //if (documents_[document_id]) { return false; }


        const vector<string> words = SplitIntoWordsNoStop(document);
        const double inv_word_count = 1.0 / words.size();
        for (const string& word : words) {
            if (!IsValidWord(word)) { return false; }  // 3. Наличие спецсимволов — то есть символов с кодами в диапазоне от 0 до 31 включительно — в тексте документов и поискового запроса.

            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
        documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings), status });
        docs_ids_.push_back(document_id);
        return true;
    }

    template <typename DocumentPredicate>
    [[nodiscard]] bool FindTopDocuments(const string& raw_query, DocumentPredicate document_predicate,
                                        vector<Document>& result) const {

        /*Указание в поисковом запросе более чем одного минуса перед словами, которых не должно быть в документах, например: кот --пушистый. В середине слов минусы разрешаются, например: иван-чай.
            Отсутствие в поисковом запросе текста после символа «минус», например кот -.*/
        //ChekTwoMinusorEmptyWord

    	for (const string& word : SplitIntoWords(raw_query)){if (!ChekTwoMinusorEmptyWord(word)){return false;} }
            	for (const string& word : SplitIntoWordsNoStop(raw_query)){if (!ChekTwoMinusorEmptyWord(word)){return false;} }
        
        //Дополнительно проверяю что слово в поисковом запросе валидно 
        for (const string& word : SplitIntoWords(raw_query)){if ( !IsValidWord(word)){return false;} }
       for (const string& word : SplitIntoWordsNoStop(raw_query)){if ( !IsValidWord(word)){return false;} }
        

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
        result = matched_documents;
        return true;
    }


    [[nodiscard]] bool FindTopDocuments(const string& raw_query, DocumentStatus status,
                                        vector<Document>& result) const {


    	return FindTopDocuments(
            raw_query, [status ](int document_id, DocumentStatus document_status, int rating ) {
                return document_status == status;
            }, result);

    }

    [[nodiscard]] bool FindTopDocuments(const string& raw_query, vector<Document>& result) const {

        return FindTopDocuments(raw_query, DocumentStatus::ACTUAL, result);

    }


    [[nodiscard]] bool MatchDocument(const string& raw_query, int document_id,
                                     tuple<vector<string>, DocumentStatus>& result) const {

        /*Указание в поисковом запросе более чем одного минуса перед словами, которых не должно быть в документах, например: кот --пушистый. В середине слов минусы разрешаются, например: иван-чай.
            Отсутствие в поисковом запросе текста после символа «минус», например кот -.*/
        //ChekTwoMinusorEmptyWord
         if (document_id < 0) { return false; }
       // 6. Попытка добавить документ с id, совпадающим с id документа, который добавился ранее.
        if (documents_.count(document_id) > 0) { return false; }

        const Query query = ParseQuery(raw_query);
        vector<string> matched_words;
        for (const string& word : query.plus_words) {
            if (!ChekTwoMinusorEmptyWord(word)){return false;} // стандартная проверка для + и минус слов
            if ( !IsValidWord(word)){return false;} // стандартная проверка для + и минус слов
            
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.push_back(word);
            }
        }
        for (const string& word : query.minus_words) {
            if (!ChekTwoMinusorEmptyWord(word)){return false;} // стандартная проверка для + и минус слов
            if (!IsValidWord(word)){return false;} // стандартная проверка для + и минус слов
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.clear();
                break;
            }
        }
        result = { matched_words, documents_.at(document_id).status };
        return true;
    }

    /*
    Также добавьте метод GetDocumentId, позволяющий получить идентификатор документа по его порядковому номеру. В случае, если порядковый номер      документа выходит за пределы от [0; кол-во документов), метод должен вернуть значение SearchServer::INVALID_DOCUMENT_ID:
    */
    int GetDocumentId(int index) const {
        //map<int, DocumentData> documents_;
        if (index< 0 || index >= GetDocumentCount()) { return SearchServer::INVALID_DOCUMENT_ID; }
        else {
        	if ( find(docs_ids_.begin(), docs_ids_.end(), index) != docs_ids_.end() )
        		{return docs_ids_.at(index); }
        	else {
        		return SearchServer::INVALID_DOCUMENT_ID;
            }
        }
    }


    int GetDocumentCount() const {
        return documents_.size();
    }

private:
    vector<int> docs_ids_;  // вектор документов
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

    // проверка что в слове нет 2 минусов подряд и что после минуса есть текст
    bool ChekTwoMinusorEmptyWord(string text) const {
        // Word shouldn't be empty
        if (text[0] == '-' && text[1] == '-') {
            return false;
        }
        else if (text[0] == '-' && text.size() == 1) { return false; }
        else { return true; }
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

// ==================== для примера =========================

void PrintDocument(const Document& document) {
    cout << "{ "s
        << "document_id = "s << document.id << ", "s
        << "relevance = "s << document.relevance << ", "s
        << "rating = "s << document.rating << " }"s << endl;
}
int main() {
    SearchServer search_server("и в на"s);
    // Явно игнорируем результат метода AddDocument, чтобы избежать предупреждения
    // о неиспользуемом результате его вызова
    (void)search_server.AddDocument(1, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });

    if (!search_server.AddDocument(1, "пушистый пёс и модный ошейник"s, DocumentStatus::ACTUAL, {1, 2})) {
        cout << "Документ не был добавлен, так как его id совпадает с уже имеющимся"s << endl;
    }
    if (!search_server.AddDocument(-1, "пушистый пёс и модный ошейник"s, DocumentStatus::ACTUAL, { 1, 2 })) {
        cout << "Документ не был добавлен, так как его id отрицательный"s << endl;
    }
    if (!search_server.AddDocument(3, "большой пёс скво\x12рец"s, DocumentStatus::ACTUAL, { 1, 3, 2 })) {
        cout << "Документ не был добавлен, так как содержит спецсимволы"s << endl;
    }
    vector<Document> documents;
    if (search_server.FindTopDocuments("--пушистый"s, documents)) {
        for (const Document& document : documents) {
            PrintDocument(document);
        }
    }
    else {
        cout << "Ошибка в поисковом запросе"s << endl;
    }
}

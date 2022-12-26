
#include <iostream>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <map>
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
            words.push_back(word);
            word = "";
        } else {
            word += c;
        }
    }
    words.push_back(word);

    return words;
}

struct Document {
    int id;
    double relevance;
    int rating;
};

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};

/* Подставьте вашу реализацию класса SearchServer сюда */
class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        const double inv_word_count = 1.0 / words.size();
        for (const string& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
        documents_.emplace(document_id,
            DocumentData{
                ComputeAverageRating(ratings),
                status
            });
    }

    template <typename DocumentPredicate>
    vector<Document> FindTopDocuments(const string& raw_query, DocumentPredicate document_predicate) const {
        const Query query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query, document_predicate);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                if (abs(lhs.relevance - rhs.relevance) < 1e-6) {
                    return lhs.rating > rhs.rating;
                } else {
                    return lhs.relevance > rhs.relevance;
                }
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        return FindTopDocuments(raw_query, [](int document_id, DocumentStatus status, int rating) { return status == DocumentStatus::ACTUAL; });
    }

    // создаю новую версию для DocumentStatus
    vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus status) const {
        return FindTopDocuments(raw_query, [&status](int document_id, DocumentStatus st, int rating) { return st == status; });
    }

    int GetDocumentCount() const {
        return documents_.size();
    }

    tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query, int document_id) const {
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
        return {matched_words, documents_.at(document_id).status};
    }

private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };

    set<string> stop_words_;
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
        return {
            text,
            is_minus,
            IsStopWord(text)
        };
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
                } else {
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
    vector<Document> FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const {
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
            matched_documents.push_back({
                document_id,
                relevance,
                documents_.at(document_id).rating
            });
        }
        return matched_documents;
    }
};

void PrintDocument(const Document& document) {
    cout << "{ "s
         << "document_id = "s << document.id << ", "s
         << "relevance = "s << document.relevance << ", "s
         << "rating = "s << document.rating
         << " }"s << endl;
}

// -------- Начало модульных тестов поисковой системы ----------

// Тест проверяет, что поисковая система исключает стоп-слова при добавлении документов
/*void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    // Сначала убеждаемся, что поиск слова, не входящего в список стоп-слов,
    // находит нужный документ
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        assert(found_docs.size() == 1);
        const Document& doc0 = found_docs[0];
        assert(doc0.id == doc_id);
    }

    // Затем убеждаемся, что поиск этого же слова, входящего в список стоп-слов,
    // возвращает пустой результат
    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        assert(server.FindTopDocuments("in"s).empty());
    }
}*/

//1. Добавление документов. Добавленный документ должен находиться по поисковому запросу, который содержит слова из документа.
void TestAddDocument() {
    SearchServer server;
    server.AddDocument(0, "белый кот и модный ошейник"s,        DocumentStatus::ACTUAL, {8, -3});
    vector<Document> document = server.FindTopDocuments("белый кот и модный ошейник"s);
    assert(server.FindTopDocuments("белый кот и модный ошейник"s)[0].id == 0);
    assert(server.FindTopDocuments("белый кот и модный ошейник"s)[0].relevance == 0);
    assert(server.FindTopDocuments("белый кот и модный ошейник"s)[0].rating == 2);
}

//2. Поддержка стоп-слов. Стоп-слова исключаются из текста документов
void TestExcludeStopWord() {
    SearchServer server;
    server.SetStopWords("и в на"s);
    server.AddDocument(0, "белый кот и в на модный ошейник"s,        DocumentStatus::ACTUAL, {8, -3});
    vector<Document> document = server.FindTopDocuments("и в на"s);
    assert(document.size() == 0);
}

//3. Поддержка минус-слов. Документы, содержащие минус-слова поискового запроса, не должны включаться в результаты поиска.
void TestMinusWord() {
    SearchServer server;
    //server.SetStopWords("и в на"s);
    server.AddDocument(0, "белый кот модный ошейник"s,        DocumentStatus::ACTUAL, {8, -3});
    server.AddDocument(1, "черный пес бульдог"s,        DocumentStatus::ACTUAL, {8, -7});
    tuple<vector<string>, DocumentStatus> match_data =  server.MatchDocument("белый кот модный -ошейник"s, 0);
    tuple<vector<string>, DocumentStatus> match_data_2 =  server.MatchDocument("черный пес -бульдог"s, 1);
    int size_vec_1 = get<0>(match_data).size();
    int size_vec_2 = get<0>(match_data_2).size();

    assert(size_vec_1 == 0);  // проверяю что с минус словом ничего не попало
    assert(size_vec_2 == 0);  // проверяю что с минус словом ничего не попало
}

// 4. Матчинг документов. При матчинге документа по поисковому запросу должны быть возвращены все слова из поискового запроса,
//присутствующие в документе. Если есть соответствие хотя бы по одному минус-слову, должен возвращаться пустой список слов.
void TestMatchDoc() {
    SearchServer server;
    //server.SetStopWords("и в на"s);
    server.AddDocument(0, "белый кот модный ошейник"s,        DocumentStatus::ACTUAL, {8, -3});
    server.AddDocument(1, "черный пес бульдог"s,        DocumentStatus::ACTUAL, {8, -7});
    tuple<vector<string>, DocumentStatus> match_data =  server.MatchDocument("белый кот модный ошейник"s, 0);

    tuple<vector<string>, DocumentStatus> match_data_2 =  server.MatchDocument("черный пес -бульдог"s, 1);
    int size = tuple_size<decltype(match_data)>::value;
    int size_vec = get<0>(match_data).size();
    string result_string = ""s;
    for (auto el : get<0>(match_data)){cout << el<< " "s ;  result_string+=el; result_string+=" "s;}
    int size_vec_2 = get<0>(match_data_2).size();

    assert(size_vec_2 == 0);  // проверяю что с минус словом ничего не попало


}

// 5. Сортировка найденных документов по релевантности. Возвращаемые при поиске документов результаты должны быть отсортированы в порядке убывания релевантности.

void TestSortDocument() {
    SearchServer server;
    server.AddDocument(0, "белый кот и модный ошейник"s,        DocumentStatus::ACTUAL, {8, -3});
    server.AddDocument(1, "белый пес модный"s,        DocumentStatus::ACTUAL, {8, -7});
    vector<Document> document = server.FindTopDocuments("белый кот и модный ошейник"s);
    assert(server.FindTopDocuments("белый кот и модный ошейник"s)[0].relevance > server.FindTopDocuments("белый кот и модный ошейник"s)[1].relevance);

}

//6. Вычисление рейтинга документов. Рейтинг добавленного документа равен среднему арифметическому оценок документа.
void TestRatingDocument() {
    SearchServer server;
    server.AddDocument(1, "белый пес модный"s,        DocumentStatus::ACTUAL, {8, 3});
    vector<Document> document = server.FindTopDocuments("белый пес модный"s);
    cout << "Print document in TestRatingDocument: "s << endl;
    assert(server.FindTopDocuments("белый кот и модный ошейник"s)[0].rating == 5);
}

//7. Фильтрация результатов поиска с использованием предиката, задаваемого пользователем.

//8 Поиск документов, имеющих заданный статус.

//9 Корректное вычисление релевантности найденных документов.
// Внимание релевантность как то не так считатет???? 
void TestRelevanceDocument() {
    SearchServer server;
    server.AddDocument(0, "белый пес модный"s,        DocumentStatus::ACTUAL, {8, 3});
    server.AddDocument(1, "белый пес старомодный"s,        DocumentStatus::ACTUAL, {8, 14});
    vector<Document> document = server.FindTopDocuments("белый пес модный"s);
    cout << "Print document in RelevanceDocument: "s << endl;

    for (auto doc : document) {
    PrintDocument(doc);}
    //assert(server.FindTopDocuments("белый кот и модный ошейник"s)[0].rating == 5);
}




/*
Разместите код остальных тестов здесь
*/

// Функция TestSearchServer является точкой входа для запуска тестов
void TestSearchServer() {
    //TestExcludeStopWordsFromAddedDocumentContent();
    TestAddDocument();
    TestExcludeStopWord();
    TestMinusWord();
    TestMatchDoc();
    TestSortDocument();
    TestRatingDocument();
    TestRelevanceDocument();
    // Не забудьте вызывать остальные тесты здесь
}

// --------- Окончание модульных тестов поисковой системы -----------

int main() {
    TestSearchServer();
    // Если вы видите эту строку, значит все тесты прошли успешно
    cout << "Search server testing finished"s << endl;
}

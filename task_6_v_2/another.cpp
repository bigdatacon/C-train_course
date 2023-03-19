   #include <numeric>
#include "search_server.h"
 
void SearchServer::AddDocument(int document_id, std::string_view document, DocumentStatus status, const std::vector<int>& ratings) {
    if ((document_id < 0) || (documents_.count(document_id) > 0)) {
        throw std::invalid_argument("Invalid document_id"s);
    }
    const auto words = SplitIntoWordsNoStop(document);
 
    const double inv_word_count = 1.0 / words.size();
    for (std::string_view word : words) {
        document_to_word_freqs_[document_id][word] += inv_word_count;
        word_to_document_freqs_[word][document_id] += inv_word_count;
    }
    documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings), status });
    document_ids_.insert(document_id);
}
 
std::vector<Document> SearchServer::FindTopDocuments(std::string_view raw_query, DocumentStatus status) const {
    return FindTopDocuments(raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
        return document_status == status;
        });
}
 
std::vector<Document> SearchServer::FindTopDocuments(std::string_view raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}
 
int SearchServer::GetDocumentCount() const {
    return documents_.size();
}
 
std::set<int> ::const_iterator SearchServer::begin() const {
    return document_ids_.begin();
}
 
std::set<int> ::const_iterator SearchServer::end() const {
    return document_ids_.end();
}
 
const std::map<std::string_view, double>& SearchServer::GetWordFrequencies(int document_id) const {
    static const std::map<std::string_view, double> emptyes{};
    return (!document_to_word_freqs_.count(document_id)) ? emptyes : document_to_word_freqs_.at(document_id);
}
 
void SearchServer::RemoveDocument(int document_id) {
 
        for (auto& [word, _] : document_to_word_freqs_[document_id]) {
            auto it = word_to_document_freqs_[word].find(document_id);
            word_to_document_freqs_[word].erase(it);
            if (word_to_document_freqs_[word].empty()) {
                word_to_document_freqs_.erase(word);
            }
        }
 
        documents_.erase(document_id);
        document_ids_.erase(document_id);
        document_to_word_freqs_.erase(document_id);
    }
 
    void SearchServer::RemoveDocument(const std::execution::sequenced_policy&, int document_id) {
        return RemoveDocument(document_id);
    }
 
    void SearchServer::RemoveDocument(const std::execution::parallel_policy&, int document_id) {
        if (!document_to_word_freqs_.count(document_id)) {
            return;
        }
 
        std::vector<std::string_view> to_delete(static_cast<int>(document_to_word_freqs_.at(document_id).size()));
 
        std::transform(std::execution::par, document_to_word_freqs_[document_id].begin(), document_to_word_freqs_[document_id].end(), to_delete.begin(), [](auto& word) {
                return word.first;            
            });
 
        std::for_each(std::execution::par, to_delete.begin(), to_delete.end(), [document_id, this](auto& temp) {word_to_document_freqs_.at(temp).erase(document_id); });
 
        documents_.erase(document_id);
        document_ids_.erase(document_id);
        document_to_word_freqs_.erase(document_id);
    }
 
std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(std::string_view raw_query, int document_id) const {
    const auto query = ParseQuery(raw_query);
    std::vector<std::string_view> matched_words;
    for (const std::string_view word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.push_back(word);
        }
    }
    for (const std::string_view word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.clear();
            break;
        }
    }
    return { matched_words, documents_.at(document_id).status };
}
 
std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(const std::execution::sequenced_policy&, std::string_view raw_query, int document_id) const {
    return MatchDocument(raw_query, document_id);
}
 
std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(const std::execution::parallel_policy&, std::string_view raw_query, int document_id) const {
 
    const auto result = ParseQuery(std::execution::par, raw_query);
    std::vector<std::string_view> matched_words(result.plus_words.size());
 
    const auto check = [this, document_id](std::string_view word) {
        const auto temp = word_to_document_freqs_.find(word);
        return temp != word_to_document_freqs_.end() &&
            temp->second.count(document_id);
    };
 
    if (std::any_of(std::execution::par,
        result.minus_words.begin(),
        result.minus_words.end(),
        check)) {
        return { {}, documents_.at(document_id).status };
    }
 
    auto end = std::copy_if(std::execution::par,
        result.plus_words.begin(),
        result.plus_words.end(),
        matched_words.begin(),
        check);
 
    std::sort(matched_words.begin(), end);
    end = std::unique(matched_words.begin(), end);
    matched_words.erase(end, matched_words.end());
 
    return { matched_words,
            documents_.at(document_id).status };
}
 
bool SearchServer::IsStopWord(std::string_view word) const {
    return stop_words_.count(word) > 0;
}
 
bool SearchServer::IsValidWord(std::string_view word) {
    return std::none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
        });
}
 
 
std::vector<std::string_view> SearchServer::SplitIntoWordsNoStop(std::string_view text) const {
    std::vector<std::string_view> words;
    for (std::string_view& word : SplitIntoWords(text)) {
        if (!IsValidWord(word)) {
            throw std::invalid_argument("Word "s + std::string(word) + " is invalid"s);
        }
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}
 
int SearchServer::ComputeAverageRating(const std::vector<int>& ratings) {
    if (ratings.empty()) {
        return 0;
    }
    int rating_sum = std::accumulate(ratings.begin(), ratings.end(), 0);
    return rating_sum / static_cast<int>(ratings.size());
}
 
SearchServer::QueryWord SearchServer::ParseQueryWord(std::string_view& text) const {
    if (text.empty()) {
        throw std::invalid_argument("Query word is empty"s);
    }
    bool is_minus = false;
    if (text[0] == '-') {
        is_minus = true;
        text = text.substr(1);
    }
    if (text.empty() || text[0] == '-' || !IsValidWord(text)) {
        throw std::invalid_argument("Query word "s + std::string(text) + " is invalid");
    }
 
    return { text, is_minus, IsStopWord(text) };
}
 
 
 
SearchServer::Query SearchServer::ParseQuery(std::string_view& text) const {
    Query result;
 
    for (std::string_view word : SplitIntoWords(text)) {
        const auto query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                result.minus_words.push_back(query_word.data);
            }
            else {
                result.plus_words.push_back(query_word.data);
            }
        }
    }
 
    std::sort(result.minus_words.begin(), result.minus_words.end());
    auto it_minus = std::unique(result.minus_words.begin(), result.minus_words.end());
    result.minus_words.erase(it_minus, result.minus_words.end());
 
    std::sort(result.plus_words.begin(), result.plus_words.end());
    auto it_plus = std::unique(result.plus_words.begin(), result.plus_words.end());
    result.plus_words.erase(it_plus, result.plus_words.end());
 
    return result;
}
 
SearchServer::Query SearchServer::ParseQuery(const std::execution::parallel_policy&, std::string_view& text) const {
    Query result;
    result.minus_words.reserve(text.size());
    result.plus_words.reserve(text.size());
    for (std::string_view word : SplitIntoWords(text)) {
        const auto query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                result.minus_words.push_back(query_word.data);
            }
            else {
                result.plus_words.push_back(query_word.data);
            }
        }
    }
 
    return result;
}
 
double SearchServer::ComputeWordInverseDocumentFreq(std::string_view& word) const {
    return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}
 
 
cpp.h
_____________________________________________________________________________________________
#pragma once
#include <tuple>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <execution>
#include <list>
 
#include "read_input_functions.h"
#include "string_processing.h"
#include "log_duration.h" 
 
using namespace std::string_literals;
 
class SearchServer {
public:
    template <typename StringContainer>
    SearchServer(const StringContainer& stop_words);
    SearchServer(const std::string& stop_words_text) : SearchServer(SplitIntoWords(stop_words_text)) {}
    SearchServer(std::string_view& stop_words_text) : SearchServer(SplitIntoWords(stop_words_text)) {}
 
    void AddDocument(int document_id, std::string_view document, DocumentStatus status, const std::vector<int>& ratings);
 
    template <typename Check>
    std::vector<Document> FindTopDocuments(std::string_view raw_query, Check check) const;
    std::vector<Document> FindTopDocuments(std::string_view raw_query, DocumentStatus status) const;
    std::vector<Document> FindTopDocuments(std::string_view raw_query) const;
 
    int GetDocumentCount() const;
 
    std::set<int> ::const_iterator begin() const;
    std::set<int> ::const_iterator end() const;
 
    const std::map<std::string_view, double>& GetWordFrequencies(int document_id) const;
 
    void RemoveDocument(int document_id);
 
 
    void RemoveDocument(const std::execution::sequenced_policy&, int document_id);
 
 
    void RemoveDocument(const std::execution::parallel_policy&, int document_id);
 
    std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(std::string_view raw_query, int document_id) const;
    std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(const std::execution::sequenced_policy&, std::string_view raw_query, int document_id) const;
    std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(const std::execution::parallel_policy&, std::string_view raw_query, int document_id) const;
 
private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };
 
    const double INACCURACY = 1e-6;
    const int MAX_RESULT_DOCUMENT_COUNT = 5;
 
    const std::set<std::string, std::less<>> stop_words_;
    std::map<std::string_view, std::map<int, double>> word_to_document_freqs_;
    std::map<int, std::map<std::string_view, double>> document_to_word_freqs_;
    std::map<int, DocumentData> documents_;
    std::set<int> document_ids_;
 
 
    bool IsStopWord(std::string_view word) const;
 
    static bool IsValidWord(std::string_view word);
 
    std::vector<std::string_view> SplitIntoWordsNoStop(std::string_view text) const;
    static int ComputeAverageRating(const std::vector<int>& ratings);
 
    struct QueryWord {
        std::string_view data;
        bool is_minus;
        bool is_stop;
    };
 
    QueryWord ParseQueryWord(std::string_view& text) const;
 
    struct Query {
        std::vector<std::string_view> plus_words;
        std::vector<std::string_view> minus_words;
    };
 
    Query ParseQuery(std::string_view& text) const;
    Query ParseQuery(const std::execution::parallel_policy&, std::string_view& text) const;
 
    double ComputeWordInverseDocumentFreq(std::string_view& word) const;
 
    template <typename DocumentPredicate>
    std::vector<Document> FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const;
};
 
template <typename StringContainer>
SearchServer::SearchServer(const StringContainer& stop_words) : stop_words_(MakeUniqueNonEmptyStrings(stop_words)) {
    if (!all_of(stop_words_.begin(), stop_words_.end(), IsValidWord)) {
        throw std::invalid_argument("Some of stop words are invalid"s);
    }
}
 
template <typename Check>
std::vector<Document> SearchServer::FindTopDocuments(std::string_view raw_query, Check check) const {
    const auto query = ParseQuery(raw_query);
    auto matched_documents = FindAllDocuments(query, check);
    sort(matched_documents.begin(), matched_documents.end(),
        [this](const Document& lhs, const Document& rhs) {
            if (std::abs(lhs.relevance - rhs.relevance) < INACCURACY) {
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
 
template <typename Check>
std::vector<Document> SearchServer::FindAllDocuments(const Query& query, Check check) const {
    std::map<int, double> document_to_relevance{};
    for (std::string_view word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
        for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) {
            const auto& document_data = documents_.at(document_id);
            if (check(document_id, document_data.status, document_data.rating)) {
                document_to_relevance[document_id] += term_freq * inverse_document_freq;
            }
        }
    }
 
    for (std::string_view word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
            document_to_relevance.erase(document_id);
        }
    }
 
    std::vector<Document> matched_documents;
    for (const auto [document_id, relevance] : document_to_relevance) {
        matched_documents.push_back(
            { document_id, relevance, documents_.at(document_id).rating });
    }
    return matched_documents;
}

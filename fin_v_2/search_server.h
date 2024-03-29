#pragma once
#include "document.h"
#include "log_duration.h"
#include "string_processing.h"
#include "read_input_functions.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <execution>
#include <functional>
#include <string_view>
#include <deque>
#include <future>
#include <chrono>
#include "concurrent_map.h" 
#include <mutex>
#include <atomic>


const int MAX_RESULT_DOCUMENT_COUNT = 5;

class SearchServer {
public:
    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stop_words);
    explicit SearchServer( std::string_view stop_words_text);
    explicit SearchServer( const std::string& stop_words_text);
    
   // void PrintDocumentFreq();
    
    void AddDocument(int document_id,  std::string_view document, DocumentStatus status, const std::vector<int>& ratings);
    
    template <typename DocumentPredicate>
    std::vector<Document> FindTopDocuments(std::string_view raw_query, DocumentPredicate document_predicate) const;

    std::vector<Document> FindTopDocuments(std::string_view raw_query, DocumentStatus status) const;

    std::vector<Document> FindTopDocuments(std::string_view raw_query) const;

    template <typename DocumentPredicate>
    std::vector<Document> FindTopDocuments(const std::execution::sequenced_policy, std::string_view raw_query, DocumentPredicate document_predicate) const;

    std::vector<Document> FindTopDocuments(const std::execution::sequenced_policy, std::string_view raw_query, DocumentStatus status) const;

    std::vector<Document> FindTopDocuments(const std::execution::sequenced_policy, std::string_view raw_query) const;

    template <typename DocumentPredicate>
    std::vector<Document> FindTopDocuments(const std::execution::parallel_policy, std::string_view raw_query, DocumentPredicate document_predicate) const;

    std::vector<Document> FindTopDocuments(const std::execution::parallel_policy, std::string_view raw_query, DocumentStatus status) const;

    std::vector<Document> FindTopDocuments(const std::execution::parallel_policy, std::string_view raw_query) const;

    int GetDocumentCount() const ;

    int GetDocumentId(int index) const ;

    //1.Откажитесь от метода GetDocumentId(int index) и вместо него определите методы begin и end.Они вернут итераторы.Итератор даст доступ к id всех документов,
    //хранящихся в поисковом сервере.Вы можете не разрабатывать собственный итератор, а применить готовый константный итератор удобного контейнера.
    std::set<int>::const_iterator begin() const;
    std::set<int>::const_iterator end() const;
    std::set<int>::iterator begin();
    std::set<int>::iterator end();
    
    
    //доплняю многопоточную версию 
    std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(std::string_view raw_query, int document_id) const;
    
    std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(const std::execution::sequenced_policy&,  std::string_view raw_query, int document_id) const;
    
    std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(const std::execution::parallel_policy& policy,  std::string_view raw_query, int document_id) const;
    /*std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(const std::execution::parallel_policy&, std::string_view raw_query, int document_id) const;*/
       
        //3. Разработайте метод удаления документов из поискового сервера
    void RemoveDocument(int document_id);
    void RemoveDocument(const std::execution::sequenced_policy&, int document_id);
    void RemoveDocument(const std::execution::parallel_policy& policy, int document_id); 
    
    std::set<int> GetDoc_ids( ){return document_ids_;} ;
    
        //2.Разработайте метод получения частот слов по id документа: 
    const std::map<std::string_view, double>& GetWordFrequencies(int document_id) const;
    
    
private:
    //std::map<int, std::map<std::string, double>> word_freqs_;
    std::deque<std::string> myDeque_ ;
    std::map<int, std::map<std::string_view, double>> word_freqs_;
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };
    const std::set<std::string, std::less<>> stop_words_;
    std::map<std::string_view, std::map<int, double>> word_to_document_freqs_;
    std::map<int, DocumentData> documents_;
    std::set<int> document_ids_;   

    bool IsStopWord( std::string_view word) const ;

    static bool IsValidWord(std::string_view word) ;

    std::vector<std::string_view> SplitIntoWordsNoStop( std::string_view text) const;

    static int ComputeAverageRating(const std::vector<int>& ratings) ;
    
    struct QueryWord {
        std::string_view data;
        bool is_minus;
        bool is_stop;
    };

    QueryWord ParseQueryWord(std::string_view text) const ;


    struct Query {
        std::vector<std::string_view> plus_words;
        std::vector<std::string_view> minus_words;
    };
    
    // Добавляю многопоточную версию ParseQuery
    Query ParseQuery(std::string_view text) const ;
    Query ParseQuery(const std::execution::sequenced_policy&,  std::string_view text) const ;
    Query ParseQuery(bool flag,  std::string_view text) const ;

    // Existence required
    double ComputeWordInverseDocumentFreq(const std::string& word) const ;
    
    template <typename DocumentPredicate>
    std::vector<Document> FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const;
    
    template <typename DocumentPredicate>
    std::vector<Document> FindAllDocuments(const std::execution::sequenced_policy, const Query& query, DocumentPredicate document_predicate) const;
    
    template <typename DocumentPredicate>
    std::vector<Document> FindAllDocuments(const std::execution::parallel_policy, const Query& query, DocumentPredicate document_predicate) const;
};

template <typename StringContainer>
SearchServer::SearchServer(const StringContainer& stop_words)
    : stop_words_(MakeUniqueNonEmptyStrings(stop_words))  // Extract non-empty stop words
{
    if (!all_of(stop_words_.begin(), stop_words_.end(), IsValidWord)) {
        throw std::invalid_argument("Some of stop words are invalid");
    }
}


template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindTopDocuments( std::string_view raw_query, DocumentPredicate document_predicate) const {

    auto start_time = std::chrono::high_resolution_clock::now();
    const auto query = ParseQuery(raw_query);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    std::cout << "Elapsed time ParseQuery / 1000 000: " << elapsed_time.count()*1000000 << " seconds\n";
    
    auto start_time_fad = std::chrono::high_resolution_clock::now();
    auto matched_documents = FindAllDocuments(query, document_predicate);
    auto end_time_fad = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time_fad = end_time_fad - start_time_fad;
    std::cout << "Elapsed time FindAllDocuments / 1000 000: " << elapsed_time_fad.count()*1000000 << " seconds\n";
    
     auto start_time_sort = std::chrono::high_resolution_clock::now();
    sort(matched_documents.begin(), matched_documents.end(), [](const Document& lhs, const Document& rhs) {
        if (std::abs(lhs.relevance - rhs.relevance) < 1e-6) {
            return lhs.rating > rhs.rating;
        } else {
            return lhs.relevance > rhs.relevance;
        }
    });
    
    auto end_time_sort = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time_sort = end_time_sort - start_time_sort;
    std::cout << "Elapsed time sort / 1000 000: " << elapsed_time_sort.count()*1000000 << " seconds\n";
   
    if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
        matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
    }
    return matched_documents;
}

template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindTopDocuments(const std::execution::sequenced_policy, std::string_view raw_query, DocumentPredicate document_predicate) const {
    return SearchServer::FindTopDocuments( raw_query, document_predicate);}


template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindTopDocuments(const std::execution::parallel_policy, std::string_view raw_query, DocumentPredicate document_predicate) const {
    auto start_time = std::chrono::high_resolution_clock::now();
    //const auto query = ParseQuery(raw_query);
    const auto query = ParseQuery(true, raw_query);
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    std::cout << "Elapsed time ParseQuery PARALLEL / 1000 000: " << elapsed_time.count()*1000000 << " seconds\n";
    
    auto start_time_fad = std::chrono::high_resolution_clock::now();
    auto matched_documents = FindAllDocuments(std::execution::par, query, document_predicate);
    auto end_time_fad = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time_fad = end_time_fad - start_time_fad;
    std::cout << "Elapsed time FindAllDocuments PARALLEL / 1000 000: " << elapsed_time_fad.count()*1000000 << " seconds\n";
    
    auto start_time_sort = std::chrono::high_resolution_clock::now();
    sort(matched_documents.begin(), matched_documents.end(), [](const Document& lhs, const Document& rhs) {
        if (std::abs(lhs.relevance - rhs.relevance) < 1e-6) {
            return lhs.rating > rhs.rating;
        } else {
            return lhs.relevance > rhs.relevance;
        }
    });
    
    auto end_time_sort = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time_sort = end_time_sort - start_time_sort;
    std::cout << "Elapsed time sort PARALLEL / 1000 000: " << elapsed_time_sort.count()*1000000 << " seconds\n";
    
    if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
        matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
    }
    return matched_documents;
}


template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const {
    std::map<int, double> document_to_relevance;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (std::string_view word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }

        std::string word_str(word);
        const double inverse_document_freq = ComputeWordInverseDocumentFreq(word_str);
        for (const auto& [document_id, term_freq] : word_to_document_freqs_.at(word)) {
            const auto& document_data = documents_.at(document_id);
            if (document_predicate(document_id, document_data.status, document_data.rating)) {
                document_to_relevance[document_id] += term_freq * inverse_document_freq;
            }
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    std::cout << "Elapsed time PLUS WORDS FINDALLDOCUMENTS  / 1000 000: " << elapsed_time.count()*1000000 << " seconds\n";
    
    auto start_time_mw = std::chrono::high_resolution_clock::now();
    for ( std::string_view word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        for (const auto& [document_id, _] : word_to_document_freqs_.at(word)) {
            document_to_relevance.erase(document_id);
        }
    }
    auto end_time_mw = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time_mw = end_time_mw - start_time_mw;
    std::cout << "Elapsed time MINUS WORDS FINDALLDOCUMENTS  / 1000 000: " << elapsed_time_mw.count()*1000000 << " seconds\n";
    
    std::vector<Document> matched_documents;
    for (const auto& [document_id, relevance] : document_to_relevance) {
        matched_documents.push_back({document_id, relevance, documents_.at(document_id).rating});
    }
    return matched_documents;
}

template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindAllDocuments(const std::execution::sequenced_policy, const Query& query, DocumentPredicate document_predicate) const {
return SearchServer::FindAllDocuments(query, document_predicate);}



template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindAllDocuments(const std::execution::parallel_policy, const Query& query, DocumentPredicate document_predicate) const {
    std::map<int, double> document_to_relevance;
    //std::map<int, std::atomic<double>> document_to_relevance;
    //ConcurrentMap<std::int, std::double> document_to_relevance;
    std::mutex freqs_mutex;
    
    for_each(
        std::execution::par,
        query.plus_words.begin(), query.plus_words.end(),
        [&,  /*&freqs_mutex,*/ document_predicate](auto word ) {
            
            if (word_to_document_freqs_.count(word) != 0) {

        std::string word_str(word);
        const double inverse_document_freq = ComputeWordInverseDocumentFreq(word_str);
        for (const auto& [document_id, term_freq] : word_to_document_freqs_.at(word)) {
            const auto& document_data = documents_.at(document_id);
            if (document_predicate(document_id, document_data.status, document_data.rating)) {
                std::lock_guard g(freqs_mutex);
                document_to_relevance[document_id] += term_freq * inverse_document_freq;
            }
        }
        
         }
         }
    );
    
    
    /*for (std::string_view word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }

        std::string word_str(word);
        const double inverse_document_freq = ComputeWordInverseDocumentFreq(word_str);
        for (const auto& [document_id, term_freq] : word_to_document_freqs_.at(word)) {
            const auto& document_data = documents_.at(document_id);
            if (document_predicate(document_id, document_data.status, document_data.rating)) {
                document_to_relevance[document_id] += term_freq * inverse_document_freq;
            }
        }
    }*/

    

    for ( std::string_view word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        for (const auto& [document_id, _] : word_to_document_freqs_.at(word)) {
            document_to_relevance.erase(document_id);
        }
    }
    


    std::vector<Document> matched_documents;
    for (const auto& [document_id, relevance] : document_to_relevance) {
        matched_documents.push_back({document_id, relevance, documents_.at(document_id).rating});
    }

    return matched_documents;
}

/*template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindAllDocuments(const std::execution::parallel_policy, const Query& query, DocumentPredicate document_predicate) const {
    std::map<int, double> document_to_relevance;
    auto start_time = std::chrono::high_resolution_clock::now();
    for (std::string_view word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }

        std::string word_str(word);
        const double inverse_document_freq = ComputeWordInverseDocumentFreq(word_str);
        for (const auto& [document_id, term_freq] : word_to_document_freqs_.at(word)) {
            const auto& document_data = documents_.at(document_id);
            if (document_predicate(document_id, document_data.status, document_data.rating)) {
                document_to_relevance[document_id] += term_freq * inverse_document_freq;
            }
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    std::cout << "Elapsed time PLUS WORDS FINDALLDOCUMENTS PARALLEL / 1000 000: " << elapsed_time.count()*1000000 << " seconds\n";
    
    auto start_time_mw = std::chrono::high_resolution_clock::now();
    for ( std::string_view word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        for (const auto& [document_id, _] : word_to_document_freqs_.at(word)) {
            document_to_relevance.erase(document_id);
        }
    }
    
    auto end_time_mw = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time_mw = end_time_mw - start_time_mw;
    std::cout << "Elapsed time MINUS WORDS FINDALLDOCUMENTS PARALLEL  / 1000 000: " << elapsed_time_mw.count()*1000000 << " seconds\n";
    
    auto start_time_dr = std::chrono::high_resolution_clock::now();
    std::vector<Document> matched_documents;
    for (const auto& [document_id, relevance] : document_to_relevance) {
        matched_documents.push_back({document_id, relevance, documents_.at(document_id).rating});
    }
    auto end_time_dr = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time_dr = end_time_dr - start_time_dr;
    std::cout << "Elapsed time DOCUMENT_TO_RELEVANCE FINDALLDOCUMENTS PARALLEL / 1000 000: " << elapsed_time.count()*1000000 << " seconds\n";
    return matched_documents;
}*/

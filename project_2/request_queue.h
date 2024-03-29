#pragma once

#include <vector>
#include <string>
#include <deque>
#include <iostream>
#include "search_server.h"

template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const SearchServer& search_server_, const std::string& raw_query,           DocumentPredicate document_predicate) {
    const auto result = search_server_.FindTopDocuments(raw_query, document_predicate);
    AddRequest(result.size());
    return result;
    }


class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status) ;
    std::vector<Document> AddFindRequest(const std::string& raw_query) ;
    int GetNoResultRequests() const ;


private:
    struct QueryResult {
        uint64_t timestamp;
        int results;
    };
    std::deque<QueryResult> requests_;
    const SearchServer& search_server_;
    int no_results_requests_;
    uint64_t current_time_;
    const static int min_in_day_ = 1440;
 
    void AddRequest(int results_num) ;
};



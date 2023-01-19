//Вставьте сюда своё решение из урока «Очередь запросов» темы «Стек, очередь, дек».
#pragma once

#include <vector>
#include <string>
#include <deque>
#include <iostream>
#include "search_server.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);
    // сделаем "обёртки" для всех методов поиска, чтобы сохранять результаты для нашей статистики
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
        // Поиск документов по запросу
        std::vector<Document> result = server_->FindTopDocuments(raw_query, document_predicate);
        // Поместить в очередь запрос и найденные документы
        enqueue(raw_query, result);
        return result;
    }
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    std::vector<Document> AddFindRequest(const std::string& raw_query);
    int GetNoResultRequests() const;
    
private:
    struct QueryResult {
        // Содержит строку запроса и документы, найденные по этому запросу
        std::string raw_query;
        std::vector<Document> documents_found;

        bool isEmpty() {return documents_found.empty();}
    };
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    // Количество пустых ответов в очереди
    int empty_queries_;
    // Сам объект поискового сервера - точнее ссылка на него
    SearchServer * server_;

    void enqueue(std::string raw_query, std::vector<Document>result);
};

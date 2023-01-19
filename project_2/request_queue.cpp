//Вставьте сюда своё решение из урока «Очередь запросов» темы «Стек, очередь, дек».
#include <vector>
#include <string>
#include "request_queue.h"

    RequestQueue::RequestQueue(const SearchServer& search_server)
    {
        // Запоминаем ссылку на объект поискового движка
        server_ = const_cast<SearchServer *> (&search_server);
        empty_queries_ = 0;
    }

    std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
        // Поиск документов по запросу
        std::vector<Document> result = server_->FindTopDocuments(raw_query, status);
        // Поместить в очередь запрос и найденные документы
        enqueue(raw_query, result);
        return result;
    }
    std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
        // Поиск документов по запросу
        std::vector<Document> result = server_->FindTopDocuments(raw_query);
        // Поместить в очередь запрос и найденные документы
        enqueue(raw_query, result);
        return result;
    }
    int RequestQueue::GetNoResultRequests() const {
        return empty_queries_;
    }

    struct RequestQueue::QueryResult {
        // Содержит строку запроса и документы, найденные по этому запросу
        std::string raw_query;
        std::vector<Document> documents_found;

        std::bool isEmpty() {return documents_found.empty();}
    };

    std::deque<QueryResult> requests_;
    /*const static*/ int min_in_day_ = 1440;
    // Количество пустых ответов в очереди
    int empty_queries_;
    // Сам объект поискового сервера - точнее ссылка на него
    SearchServer * server_;

    void RequestQueue::enqueue(std::string raw_query, std::vector<Document>result) {
        // Поместить в очередь запрос и найденные документы
        requests_.push_back({raw_query, result});
        if (result.empty()) {
            empty_queries_++;
        }
        while (requests_.size() > min_in_day_) {
            // Если очередь переполнена, то убрать элемент из начала
            if (requests_.front().isEmpty()) {
                // И если удаляемый элемент пуст то уменьшить общее количество пустых запросов
                empty_queries_--;
            }
            requests_.pop_front();
        }
    }
};

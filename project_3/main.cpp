//Вставьте сюда своё решение из урока «Очередь запросов» темы «Стек, очередь, дек».
#include "document.h"
#include "search_server.h"
#include "request_queue.h"
#include <iostream>
#include <string>
#include "paginator.h"

using namespace std;






int main() {
    SearchServer search_server("and in at"s);
    RequestQueue request_queue(search_server);
    search_server.AddDocument(1, "curly cat curly tail"s, DocumentStatus::ACTUAL, {7, 2, 7});
    search_server.AddDocument(2, "curly dog and fancy collar"s, DocumentStatus::ACTUAL, {1, 2, 3});
    search_server.AddDocument(3, "big cat fancy collar "s, DocumentStatus::ACTUAL, {1, 2, 8});
    search_server.AddDocument(4, "big dog sparrow Eugene"s, DocumentStatus::ACTUAL, {1, 3, 2});
    search_server.AddDocument(5, "big dog sparrow Vasiliy"s, DocumentStatus::ACTUAL, {1, 1, 1});
    // Сначала никаких запросов нет
    //cout << "Total empty requests: "s << request_queue.GetNoResultRequests() << endl;
    // 1439 запросов с нулевым результатом
    for (int i = 0; i < 1439; ++i) {
        request_queue.AddFindRequest("empty request"s);
    }
    //cout << "Total empty requests: "s << request_queue.GetNoResultRequests() << endl;
    // все еще 1439 запросов с нулевым результатом
    request_queue.AddFindRequest("curly dog"s);
    //cout << "Total empty requests: "s << request_queue.GetNoResultRequests() << endl;
    // новые сутки, первый запрос удален, 1438 запросов с нулевым результатом
    request_queue.AddFindRequest("big collar"s);
    //cout << "Total empty requests: "s << request_queue.GetNoResultRequests() << endl;
    // первый запрос удален, 1437 запросов с нулевым результатом
    request_queue.AddFindRequest("sparrow"s);
    cout << "Total empty requests: "s << request_queue.GetNoResultRequests() << endl;
    return 0;
}


/*int main() {
    SearchServer server(""s);
    server.AddDocument(1, "A quick brown fox jumps over a lazy dog"s, DocumentStatus::ACTUAL, {4, 5});
    server.AddDocument(2, "Hello everyone"s, DocumentStatus::ACTUAL, {3, 5});
    vector<Document> docs = server.FindTopDocuments("a dog"s);
    cout << "a dog" << endl;
    for (auto doc: docs) {
        cout << doc << endl;
    }
    docs = server.FindTopDocuments("Hello"s);
    cout << "Hello" << endl;
    for (auto doc: docs) {
        cout << doc << endl;
    }
    docs = server.FindTopDocuments("None"s);
    cout << "None" << endl;
    for (auto doc: docs) {
        cout << doc << endl;
    }
    docs = server.FindTopDocuments("Hello -everyone"s);
    cout << "Hello -everyone" << endl;
    for (auto doc: docs) {
        cout << doc << endl;
    }
    docs = server.FindTopDocuments("dog -fox"s);
    cout << "dog -fox" << endl;
    for (auto doc: docs) {
        cout << doc << endl;
    }
    docs = server.FindTopDocuments("dog -hello"s);
    cout << "dog -hello" << endl;
    for (auto doc: docs) {
        cout << doc << endl;
    }
    return 0;
}*/
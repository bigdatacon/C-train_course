#include "search_server.h"
#include "process_queries.h"
#include <iostream>
#include <string>
#include <vector>
#include <string_view>
using namespace std;


/*void PrintMatchDocumentResultUTest(int document_id, const std::vector<std::string_view>& words,
                                   DocumentStatus status) {
    std::cout << "{ "
              << "document_id = " << document_id << ", "
              << "status = " << static_cast<int>(status) << ", "
              << "words =";
    for (const string_view& word : words) {
        std::cout << ' ' << word;
    }
    std::cout << "}" << std::endl;
}
 
void PrintDocumentUTest(const Document& document) {
    std::cout << "{ "
              << "document_id = " << document.id << ", "
              << "relevance = " << document.relevance << ", "
              << "rating = " << document.rating << " }" << std::endl;
}
 
void TestMatch() {
const std::vector<int> ratings1 = {1, 2, 3, 4, 5};
const std::vector<int> ratings2 = {-1, -2, 30, -3, 44, 5};
const std::vector<int> ratings3 = {12, -20, 80, 0, 8, 0, 0, 9, 67};
const std::vector<int> ratings4 = {7, 0, 3, -49, 5};
const std::vector<int> ratings5 = {81, -6, 7, 94, -7};
const std::vector<int> ratings6 = {41, 8, -7, 897, 5};
const std::vector<int> ratings7 = {543, 0, 43, 4, -5};
const std::vector<int> ratings8 = {91, 7, 3, -88, 56};
const std::vector<int> ratings9 = {0, -87, 93, 66, 5};
const std::vector<int> ratings10 = {11, 2, -43, 4, 895};
    std::string stop_words = "и в на";
    SearchServer search_server(stop_words);
 
    search_server.AddDocument(0, "белый кот и модный ошейник", DocumentStatus::ACTUAL, ratings1);
    search_server.AddDocument(1, "пушистый кот пушистый хвост", DocumentStatus::ACTUAL, ratings2);
    search_server.AddDocument(2, "ухоженный пёс выразительные глаза", DocumentStatus::ACTUAL,
                              ratings3);
    search_server.AddDocument(3, "белый модный кот", DocumentStatus::IRRELEVANT, ratings1);
    search_server.AddDocument(4, "пушистый кот пёс", DocumentStatus::IRRELEVANT, ratings2);
    search_server.AddDocument(5, "ухоженный ошейник выразительные глаза",
                              DocumentStatus::IRRELEVANT, ratings3);
    search_server.AddDocument(6, "кот и ошейник", DocumentStatus::BANNED, ratings1);
    search_server.AddDocument(7, "пёс и хвост", DocumentStatus::BANNED, ratings2);
    search_server.AddDocument(8, "модный пёс пушистый хвост", DocumentStatus::BANNED, ratings3);
    search_server.AddDocument(9, "кот пушистый ошейник", DocumentStatus::REMOVED, ratings1);
    search_server.AddDocument(10, "ухоженный кот и пёс", DocumentStatus::REMOVED, ratings2);
    search_server.AddDocument(11, "хвост и выразительные глаза", DocumentStatus::REMOVED, ratings3);
 
    const std::string query = "пушистый ухоженный кот -ошейник";
    const auto documents = search_server.FindTopDocuments(query);
 
    std::cout << "Top documents for query:" << std::endl;
    for (const Document& document : documents) {
        PrintDocumentUTest(document);
    }
 
    std::cout << "Documents' statuses SEQUENCE :" << std::endl;
    const int document_count = search_server.GetDocumentCount();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        const auto [words, status] = search_server.MatchDocument(execution::seq, query, document_id);
        std::cout<<"SIZE SEQUENCE: "<<words.size()<<std::endl;
        PrintMatchDocumentResultUTest(document_id, words, status);
    }
    
    std::cout << "Documents' statuses PARALLEL :" << std::endl;
    const int document_count_par = search_server.GetDocumentCount();
    for (int document_id = 0; document_id < document_count_par; ++document_id) {
        const auto [words, status] = search_server.MatchDocument(execution::par, query, document_id);
        std::cout<<"SIZE PARALLEL: "<<words.size()<<std::endl;
        PrintMatchDocumentResultUTest(document_id, words, status);
    }
}
*/
int main() {
    //TestMatch();
    
    // Должен быть такой вывод:
    
    /*1 words for document 1
2 words for document 2
0 words for document 3 */
    
    //std::cout << " MAIN MAIN :" << std::endl;
    
    SearchServer search_server("and with"s);
    
    int id = 0;
    for (
        const string& text : {
            "funny pet and nasty rat"s,
            "funny pet with curly hair"s,
            "funny pet and not very nasty rat"s,
            "pet with rat and rat and rat"s,
            "nasty rat with curly hair"s,
        }
    ) {
        search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, {1, 2});
    }

    const string query = "curly and funny -not"s;
    //std::string_view sw(query.data(), query.length());
    std::string_view sw(query);
    //convert string to string view 
    
    {
        const auto [words, status] = search_server.MatchDocument(sw, 1);
        cout << words.size() << " words for document 1 with sv : "s << endl;
        // 1 words for document 1
    }
    
    {
        const auto [words, status] = search_server.MatchDocument(query, 1);
        cout << words.size() << " words for document 1"s << endl;
        // 1 words for document 1
    }

    {
        const auto [words, status] = search_server.MatchDocument(execution::seq, query, 2);
        cout << words.size() << " words for document 2"s << endl;
        // 2 words for document 2
    }
    
    {
        const auto [words, status] = search_server.MatchDocument(execution::par, query, 2);
        cout << words.size() << " words for document 2 par "s << endl;
        // 2 words for document 2
    }
    


    {
        const auto [words, status] = search_server.MatchDocument(execution::par, query, 3);
        cout << words.size() << " words for document 3"s << endl;
        // 0 words for document 3
    }

    return 0;
}
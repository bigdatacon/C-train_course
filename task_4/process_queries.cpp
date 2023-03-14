#include "process_queries.h"
#include <vector>
#include <string>
#include <algorithm>
#include <execution>
#include <iostream>
#include <functional>

// Подсказка 
/*
Достаточно применить функцию transform, которая запишет результат в вектор, заранее созданный с нужной длиной.
*/

std::vector<std::vector<Document>> ProcessQueries(
    const SearchServer& search_server,
    const std::vector<std::string>& queries)
{
    std::vector<Document> results;
    std::vector<std::vector<Document>> itg(queries.size());
    transform(std::execution::par, queries.begin(), queries.end(), itg.begin(),     [&search_server](std::string string_for_find) {return search_server.FindTopDocuments(string_for_find); } );

return itg;    
    
}

//1 так не работает
/*std::vector<Document> ProcessQueriesJoined(
    const SearchServer& search_server,
    const std::vector<std::string>& queries)
{
    std::vector<Document> results;
    transform(std::execution::par, queries.begin(), queries.end(), results.begin(),     [&search_server](std::string string_for_find) {return search_server.FindTopDocuments(string_for_find); } );

return results;    
    
}*/

//2 переиспользую функцию выше - так не работает прямым проходом 
std::vector<Document> ProcessQueriesJoined(
    const SearchServer& search_server,
    const std::vector<std::string>& queries)
{
    std::vector<Document> results;

        auto documents = ProcessQueries(search_server, queries);
        for (auto el : documents){ 
            transform(std::execution::par, el.begin(), el.end(), results.begin(),     [results](Document doc) {return results.push_back(doc); } );
        }

return results;    
    
}

//3 переиспользую функцию выше -через лябмду но иначе - тоже не работает
/*std::vector<Document> ProcessQueriesJoined(
    const SearchServer& search_server,
    const std::vector<std::string>& queries)
{
    std::vector<Document> results;

    auto documents = ProcessQueries(search_server, queries);

    transform(std::execution::par, documents.begin(), documents.end(), results.begin(),     [results](Document doc) {return results.push_back(doc); } );
    }

return results;    
    
}*/


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
    std::vector<Document> results(queries.size());
    transform(std::execution::par, queries.begin(), queries.end(), results.begin(), [&search_server](std::string string_for_find) { return search_server.FindTopDocuments(string_for_find, [](int document_id, DocumentStatus status, int rating) {
        return status == DocumentStatus::ACTUAL; }); });  // записываю результат работы поискадокументов в вектор
return results;    
    
    
};

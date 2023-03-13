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
        
        // тривиальное решение ниже работает
        /*std::vector<std::vector<Document>> documents_lists;
        for (const std::string& query : queries) {
        documents_lists.push_back(search_server.FindTopDocuments(query));
    } 
    return documents_lists;*/
    
    
}

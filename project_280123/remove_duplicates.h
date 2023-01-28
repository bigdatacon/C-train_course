// в качестве заготовки кода используйте последнюю версию своей поисковой системы
// в качестве заготовки кода используйте последнюю версию своей поисковой системы

//4. Вне класса сервера разработайте функцию поиска и удаления дубликатов :
#pragma once
#include "search_server.h"
#include "document.h"
#include "string_processing.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <deque>
#include <numeric>


void RemoveDuplicates(SearchServer& search_server);

// отдельно созадю метод ADDDOCUMENT
void SearchServer::AddDocument(int document_id, const string& document, DocumentStatus status,
                     const std::vector<int>& ratings); 

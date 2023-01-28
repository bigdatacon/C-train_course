// в качестве заготовки кода используйте последнюю версию своей поисковой системы
// в качестве заготовки кода используйте последнюю версию своей поисковой системы
#include "search_server.h"
#include "remove_duplicates.h"
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


//4. Вне класса сервера разработайте функцию поиска и удаления дубликатов :

void RemoveDuplicates(SearchServer::SearchServer& search_server) {
    std::map<std::string, double> m; // то что приходит из GetWordFrequencies(document_id)
    std::vector<std::string> key_words;  // только слова документа без id 
    //pair<int, vector<string>> document_struct; // структура слов для 1 документа по id 
    std::vector <std::pair<int, std::vector<std::string>>>  all_document_struct; // структура слов для всех документов по id 

    for (const int document_id :search_server) {
        std::map<std::string, double> id_freq = SearchServer::GetWordFrequencies(document_id); // получаю частоту слов для id
        // записываю только слова для id В вектор
        for (std::map<std::string, double>::iterator it = id_freq.begin(); it != id_freq.end(); ++it) {
            key_words.push_back(it->first);
        }
        // добавляю id : vector (слов) в итоговую пару document_struct
        all_document_struct.push_back(std::pair(document_id, key_words);  // добавляю в вектора для каждого id его струткуру документов 

    }
    // иду по вектору all_document_struct и нахожу равные key_words
    for (auto i = 0; i < all_document_struct.size() - 1; ++i) {
        for (auto j = 1; j < all_document_struct.size(); ++j)
            if (all_document_struct[i].second == all_document_struct[j].second) {
                cout << "Found duplicate document id " << all_document_struct[j].first << endl;
                SearchServer::RemoveDocument(all_document_struct[j].first);

            }

    }


}
                                      
// в качестве заготовки кода используйте последнюю версию своей поисковой системы
// в качестве заготовки кода используйте последнюю версию своей поисковой системы
#include "search_server.h"
#include "remove_duplicates.h"
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


//4. Вне класса сервера разработайте функцию поиска и удаления дубликатов :

void RemoveDuplicates(SearchServer::SearchServer& search_server) {
    std::map<std::string, double> m; // то что приходит из GetWordFrequencies(document_id)
    std::vector<std::string> key_words;  // только слова документа без id 
    //pair<int, vector<string>> document_struct; // структура слов для 1 документа по id 
    std::vector <std::pair<int, std::vector<std::string>>>  all_document_struct; // структура слов для всех документов по id 

    for (const int document_id :search_server) {
        std::map<std::string, double> id_freq = SearchServer::GetWordFrequencies(document_id); // получаю частоту слов для id
        // записываю только слова для id В вектор
        for (std::map<std::string, double>::iterator it = id_freq.begin(); it != id_freq.end(); ++it) {
            key_words.push_back(it->first);
        }
        // добавляю id : vector (слов) в итоговую пару document_struct
        all_document_struct.push_back(std::pair(document_id, key_words);  // добавляю в вектора для каждого id его струткуру документов 

    }
    // иду по вектору all_document_struct и нахожу равные key_words
    for (auto i = 0; i < all_document_struct.size() - 1; ++i) {
        for (auto j = 1; j < all_document_struct.size(); ++j)
            if (all_document_struct[i].second == all_document_struct[j].second) {
                cout << "Found duplicate document id " << all_document_struct[j].first << endl;
                SearchServer::RemoveDocument(all_document_struct[j].first);

            }

    }


}
                                      
                                      
                                      
// отдельно созадю метод ADDDOCUMENT
void SearchServer::AddDocument(int document_id, const string& document, DocumentStatus status,
                     const vector<int>& ratings) {
        if ((document_id < 0) || (SearchServer::documents_.count(document_id) > 0)) {
            throw invalid_argument("Invalid document_id"s);
        }
        const auto words = SearchServer::SplitIntoWordsNoStop(document);
        const double inv_word_count = 1.0 / words.size();
        for (const std::string& word : words) {
            SearchServer::word_to_document_freqs_[word][document_id] += inv_word_count;
        }
        SearchServer::documents_.emplace(document_id, SearchServer::DocumentData{ComputeAverageRating(ratings), status});
        SearchServer::document_ids_.push_back(document_id);
    }

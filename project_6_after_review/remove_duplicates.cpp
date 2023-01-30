#include "search_server.h"
#include "remove_duplicates.h"
#include "document.h"
#include "string_processing.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <deque>
#include <numeric>


void RemoveDuplicates(SearchServer& search_server) {
		for (int i = 0; i < (search_server.end() - search_server.begin()); ++i) {
			for (int j = i + 1; j < (search_server.end() - search_server.begin()); ++j) {
				if (search_server.GetId_words()[i] == search_server.GetId_words()[j]) {
					search_server.RemoveDocument(search_server.GetDoc_ids()[j]);
				}
			}
		}

	}

/*void RemoveDuplicates(SearchServer& search_server) {
  std::map<std::string, double> m; // то что приходит из GetWordFrequencies(document_id)
  //pair<int, vector<string>> document_struct; // структура слов для 1 документа по id 
  std::map <int, std::pair<int, std::vector<std::string>>>  all_document_struct; // структура слов для всех документов по id 

  for (const int document_id: search_server.GetDoc_ids()) {
    std::vector<std::string> key_words;  // только слова документа без id 
    std::map<std::string, double> id_freq = search_server.GetWordFrequencies(document_id); // получаю частоту слов для id
    // записываю только слова для id В вектор
    for (std::map<std::string, double>::iterator it = id_freq.begin(); it != id_freq.end(); ++it) {
      key_words.push_back(it->first);
    }
    // добавляю id : vector (слов) в итоговую пару document_struct
    all_document_struct[document_id] = std::make_pair(document_id, key_words);  // добавляю в вектора для каждого id его струткуру документов 

  }

  for (auto i = search_server.begin(); i != search_server.end(); i++) {
    for (auto j = i + 1; j != search_server.end(); j++) {
      if (all_document_struct[*i].second == all_document_struct[*j].second) {
        std::cout << "Found duplicate document id " << all_document_struct[*j].first << std::endl;
        search_server.RemoveDocument(all_document_struct[*j].first);
      }
    }
  }
}
*/

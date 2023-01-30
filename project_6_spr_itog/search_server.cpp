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


SearchServer::SearchServer(const std::string& stop_words_text)
	: SearchServer(
		SplitIntoWords(stop_words_text))  // Invoke delegating constructor from string container
{
}

void SearchServer::AddDocument(int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& ratings) {
	if ((document_id < 0) || (documents_.count(document_id) > 0)) {
		throw std::invalid_argument("Invalid document_id");
	}
	const auto words = SplitIntoWordsNoStop(document);

	const double inv_word_count = 1.0 / words.size();
	for (const std::string& word : words) {
		word_to_document_freqs_[word][document_id] += inv_word_count;
        word_freqs_[document_id][word] += inv_word_count; // добавил заполнене частов для id документа в разбивке           //по словам
	}
	documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings), status });
	document_ids_.push_back(document_id);

}

std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query, DocumentStatus status) const {
	return FindTopDocuments(raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
		return document_status == status;
		});
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query) const {
	return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}

int SearchServer::GetDocumentCount() const {
	return documents_.size();
}

int SearchServer::GetDocumentId(int index) const {
	return document_ids_.at(index);
}
//1.Откажитесь от метода GetDocumentId(int index) и вместо него определите методы begin и end.Они вернут итераторы.Итератор даст доступ к id всех документов,
//хранящихся в поисковом сервере.Вы можете не разрабатывать собственный итератор, а применить готовый константный итератор удобного контейнера.
std::vector<int>::const_iterator SearchServer::begin() const
{
	return document_ids_.begin();
}

std::vector<int>::const_iterator SearchServer::end() const
{
	return document_ids_.end();
}

std::vector<int>::iterator SearchServer::begin()
{
	return document_ids_.begin();
}

std::vector<int>::iterator SearchServer::end()
{
	return document_ids_.end();
}

std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(const std::string& raw_query, int document_id) const {
	LOG_DURATION("SearchServer::MatchDocument", std::cerr);
	const auto query = ParseQuery(raw_query);
	std::vector<std::string> matched_words;
	for (const std::string& word : query.plus_words) {
		if (word_to_document_freqs_.count(word) == 0) {
			continue;
		}
		if (word_to_document_freqs_.at(word).count(document_id)) {
			matched_words.push_back(word);
		}
	}
	for (const std::string& word : query.minus_words) {
		if (word_to_document_freqs_.count(word) == 0) {
			continue;
		}
		if (word_to_document_freqs_.at(word).count(document_id)) {
			matched_words.clear();
			break;
		}
	}
	return { matched_words, documents_.at(document_id).status };
}

bool SearchServer::IsStopWord(const std::string& word) const {
	return stop_words_.count(word) > 0;
}

bool SearchServer::IsValidWord(const std::string& word) {
	// A valid word must not contain special characters
	return none_of(word.begin(), word.end(), [](char c) {
		return c >= '\0' && c < ' ';
		});
}

std::vector<std::string> SearchServer::SplitIntoWordsNoStop(const std::string& text) const {
	std::vector<std::string> words;
	for (const std::string& word : SplitIntoWords(text)) {
		if (!IsValidWord(word)) {
			throw std::invalid_argument("Word " + word + " is invalid");
		}
		if (!IsStopWord(word)) {
			words.push_back(word);
		}
	}
	return words;
}

int SearchServer::ComputeAverageRating(const std::vector<int>& ratings) {
	if (ratings.empty()) {
		return 0;
	}
	int rating_sum = std::accumulate(ratings.begin(), ratings.end(), 0);
	return rating_sum / static_cast<int>(ratings.size());
}


SearchServer::QueryWord SearchServer::ParseQueryWord(const std::string& text) const {
	if (text.empty()) {
		throw std::invalid_argument("Query word is empty");
	}
	std::string word = text;
	bool is_minus = false;
	if (word[0] == '-') {
		is_minus = true;
		word = word.substr(1);
	}
	if (word.empty() || word[0] == '-' || !IsValidWord(word)) {
		throw std::invalid_argument("Query word " + text + " is invalid");
	}

	return { word, is_minus, IsStopWord(word) };
}
    // 5. создаю публичную функцию чтобы вернуть приватную переменную вне класса 
//std::vector<int> GetDoc_ids()  { return document_ids_;}

SearchServer::Query SearchServer::ParseQuery(const std::string& text) const {
	Query result;
	for (const std::string& word : SplitIntoWords(text)) {
		const auto query_word = ParseQueryWord(word);
		if (!query_word.is_stop) {
			if (query_word.is_minus) {
				result.minus_words.insert(query_word.data);
			}
			else {
				result.plus_words.insert(query_word.data);
			}
		}
	}
	return result;
}

// Existence required
double SearchServer::ComputeWordInverseDocumentFreq(const std::string& word) const {
	return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}


//2.Разработайте метод получения частот слов по id документа: 
//Если документа не существует, возвратите ссылку на пустой map
const std::map<std::string, double>& SearchServer::GetWordFrequencies(int document_id) const {
	static std::map<std::string, double> word_freqs;
	if (word_freqs_.count(document_id) == 0) {
		return word_freqs;
	}
	word_freqs = word_freqs_.at(document_id);

	return word_freqs;
}


void SearchServer::RemoveDocument(int document_id) {
  word_freqs_.erase(document_id);
  for (auto el : word_to_document_freqs_) {
    auto it = el.second.find(document_id);
    if (it != el.second.end()) {
      el.second.erase(it);
    }
  }
  documents_.erase(document_id);
  //std::remove(document_ids_.begin(), document_ids_.end(), document_id);
  auto it = std::remove(document_ids_.begin(), document_ids_.end(), document_id);
  document_ids_.erase(it, document_ids_.end());
}

//4. Вне класса сервера разработайте функцию поиска и удаления дубликатов :
/*void SearchServer::RemoveDuplicates() {
  std::map<std::string, double> m; // то что приходит из GetWordFrequencies(document_id)
  //pair<int, vector<string>> document_struct; // структура слов для 1 документа по id 
  std::vector <std::pair<int, std::vector<std::string>>>  all_document_struct; // структура слов для всех документов по id 

  for (const int document_id: document_ids_) {
    std::vector<std::string> key_words;  // только слова документа без id 
    std::map<std::string, double> id_freq = GetWordFrequencies(document_id); // получаю частоту слов для id
    // записываю только слова для id В вектор
    for (std::map<std::string, double>::iterator it = id_freq.begin(); it != id_freq.end(); ++it) {
      key_words.push_back(it->first);
    }
    // добавляю id : vector (слов) в итоговую пару document_struct
    all_document_struct.push_back(std::make_pair(document_id, key_words));  // добавляю в вектора для каждого id его струткуру документов 

  }
  // иду по вектору all_document_struct и нахожу равные key_words
  for (auto i = 0; i < (int) all_document_struct.size() - 1; ++i) {
    for (auto j = i + 1; j < (int) all_document_struct.size(); ++j) {
      if (all_document_struct[i].second == all_document_struct[j].second) {
        std::cout << "Found duplicate document id " << all_document_struct[j].first << std::endl;
        RemoveDocument(all_document_struct[j].first);
        all_document_struct.erase(all_document_struct.begin() + j);
        j--;
      }
    }
  }


}*/


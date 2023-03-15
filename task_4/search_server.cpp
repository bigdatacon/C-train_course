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
	document_ids_.insert(document_id);

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
	//return document_ids_.at(index);
    std::set<int>::iterator it = document_ids_.begin();
    std::advance(it, index);
    return *it;
}
//1.Откажитесь от метода GetDocumentId(int index) и вместо него определите методы begin и end.Они вернут итераторы.Итератор даст доступ к id всех документов,
//хранящихся в поисковом сервере.Вы можете не разрабатывать собственный итератор, а применить готовый константный итератор удобного контейнера.
std::set<int>::const_iterator SearchServer::begin() const
{
	return document_ids_.begin();
}

std::set<int>::const_iterator SearchServer::end() const
{
	return document_ids_.end();
}

std::set<int>::iterator SearchServer::begin()
{
	return document_ids_.begin();
}

std::set<int>::iterator SearchServer::end()
{
	return document_ids_.end();
}

std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(const std::string& raw_query, int document_id) const {
	
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
	return word_freqs_.at(document_id);
}

// Версия без политики
/*void SearchServer::RemoveDocument(int document_id) {
  word_freqs_.erase(document_id);
  for (auto &word: SearchServer:: word_to_document_freqs_) {
        bool is_document_present = word.second.count(document_id);
        if (is_document_present) {
            word.second.erase(document_id);
        }
    }  
  
  documents_.erase(document_id);
  document_ids_.erase(document_id);
}*/

 void SearchServer::RemoveDocument(int document_id){
    document_ids_.erase(document_id);
    documents_.erase(document_id);
    word_freqs_.erase(document_id);
    for(auto &[key, value]:word_to_document_freqs_){
        value.erase(document_id);
    }
 }


// ПОследовательная политика вызывает версию без политики
void SearchServer::RemoveDocument(const std::execution::sequenced_policy&, int document_id){return RemoveDocument(document_id);}


void SearchServer::RemoveDocument(const std::execution::parallel_policy& policy, int document_id) {
    if (document_ids_.count(document_id) == 0) return;
    
    std::vector<const std::string*> words_for_erase; // создаю вектор указателей 
    
    words_for_erase.reserve(word_to_document_freqs_.size()); // резервирую размер вектора по размеру map из которого нужно удалить id 
    words_for_erase.resize(word_to_document_freqs_.size());
    // записываю указатели на слова в вектор   
    transform(
        policy,
        word_freqs_.at(document_id).begin(),
        word_freqs_.at(document_id).end(),
        words_for_erase.begin(),
        [this](const auto & temp) {
                return  new std::string(temp.first); /*const_cast<std::string *> (&temp.first); */
            }
    );
    auto p = [this](const std::string* t){return word_to_document_freqs_.erase(*t);};
    std::for_each(
        policy,
        words_for_erase.begin(),
        words_for_erase.end(),
        p
    );

    word_freqs_.erase(document_id);
    documents_.erase(document_id);
    document_ids_.erase(document_id);
}



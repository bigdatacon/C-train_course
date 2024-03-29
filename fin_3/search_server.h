#pragma once
#include "document.h"
#include "log_duration.h"
#include "string_processing.h"
#include "read_input_functions.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <execution>
#include <functional>
#include <string_view>
#include <deque>
#include <future>

#include "concurrent_map.h" 
#include <mutex>
#include <atomic>
#include <thread>
#include <type_traits>


const int MAX_RESULT_DOCUMENT_COUNT = 5;
const unsigned int NUM_CPUS = std::thread::hardware_concurrency();
const double ERR_NUM = 1e-6;

class SearchServer {
public:
	template <typename StringContainer>
	explicit SearchServer(const StringContainer& stop_words);
	explicit SearchServer(std::string_view stop_words_text);
	explicit SearchServer(const std::string& stop_words_text);


	void AddDocument(int document_id, std::string_view document, DocumentStatus status, const std::vector<int>& ratings);

	template <typename DocumentPredicate>
	std::vector<Document> FindTopDocuments(std::string_view raw_query, DocumentPredicate document_predicate) const;

	std::vector<Document> FindTopDocuments(std::string_view raw_query, DocumentStatus status) const;

	std::vector<Document> FindTopDocuments(std::string_view raw_query) const;


	template <typename DocumentPredicate, typename Policy>
	std::vector<Document> FindTopDocuments(const Policy&, std::string_view raw_query, DocumentPredicate document_predicate) const;

	template < typename Policy>
	std::vector<Document> FindTopDocuments(const Policy&, std::string_view raw_query, DocumentStatus status) const;

	template < typename Policy>
	std::vector<Document> FindTopDocuments(const Policy&, std::string_view raw_query) const;

	int GetDocumentCount() const;

	int GetDocumentId(int index) const;

	std::set<int>::const_iterator begin() const;
	std::set<int>::const_iterator end() const;
	std::set<int>::iterator begin();
	std::set<int>::iterator end();

	std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(std::string_view raw_query, int document_id) const;
	std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(const std::execution::sequenced_policy&, std::string_view raw_query, int document_id) const;

	std::tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(const std::execution::parallel_policy& policy, std::string_view raw_query, int document_id) const;

	void RemoveDocument(int document_id);
	void RemoveDocument(const std::execution::sequenced_policy&, int document_id);
	void RemoveDocument(const std::execution::parallel_policy& policy, int document_id);

	std::set<int> GetDoc_ids() { return document_ids_; };

	const std::map<std::string_view, double>& GetWordFrequencies(int document_id) const;


private:
	std::deque<std::string> myDeque_;
	std::map<int, std::map<std::string_view, double>> word_freqs_;
	struct DocumentData {
		int rating;
		DocumentStatus status;
	};
	const std::set<std::string, std::less<>> stop_words_;
	std::map<std::string_view, std::map<int, double>> word_to_document_freqs_;
	std::map<int, DocumentData> documents_;
	std::set<int> document_ids_;
	bool IsStopWord(std::string_view word) const;
	static bool IsValidWord(std::string_view word);
	std::vector<std::string_view> SplitIntoWordsNoStop(std::string_view text) const;
	static int ComputeAverageRating(const std::vector<int>& ratings);
	struct QueryWord {
		std::string_view data;
		bool is_minus;
		bool is_stop;
	};
	QueryWord ParseQueryWord(std::string_view text) const;
	struct Query {
		std::vector<std::string_view> plus_words;
		std::vector<std::string_view> minus_words;
	};

	Query ParseQuery(std::string_view text) const;
	Query ParseQuery(const std::execution::sequenced_policy&, std::string_view text) const;
	Query ParseQuery(bool flag, std::string_view text) const;

	double ComputeWordInverseDocumentFreq(const std::string_view& word) const;

	template <typename DocumentPredicate>
	std::vector<Document> FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const;

	template <typename DocumentPredicate, typename Policy>
	std::vector<Document> FindAllDocuments(const Policy&, const Query& query, DocumentPredicate document_predicate) const;
};

template <typename StringContainer>
SearchServer::SearchServer(const StringContainer& stop_words)
	: stop_words_(MakeUniqueNonEmptyStrings(stop_words))
{
	if (!all_of(stop_words_.begin(), stop_words_.end(), IsValidWord)) {
		throw std::invalid_argument("Some of stop words are invalid");
	}
}

template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindTopDocuments(std::string_view raw_query, DocumentPredicate document_predicate) const {
    return FindTopDocuments( std::execution::seq, raw_query, document_predicate);
    }
    // то что было раньше до правок замечания
    /*
    const auto query = ParseQuery(raw_query);
	auto matched_documents = FindAllDocuments(query, document_predicate);
	sort(matched_documents.begin(), matched_documents.end(), [](const Document& lhs, const Document& rhs) {
		if (std::abs(lhs.relevance - rhs.relevance) < ERR_NUM) {
			return lhs.rating > rhs.rating;
		}
		else {
			return lhs.relevance > rhs.relevance;
		}
		});

	if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
		matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
	}
	return matched_documents;
}*/


template <typename DocumentPredicate, typename Policy>
std::vector<Document> SearchServer::FindTopDocuments(const Policy& policy, std::string_view raw_query, DocumentPredicate document_predicate) const {
	const auto query = ParseQuery(raw_query);
	auto matched_documents = FindAllDocuments(policy, query, document_predicate);
	sort(policy, matched_documents.begin(), matched_documents.end(), [](const Document& lhs, const Document& rhs) {
		if (std::abs(lhs.relevance - rhs.relevance) < 1e-6) {
			return lhs.rating > rhs.rating;
		}
		else {
			return lhs.relevance > rhs.relevance;
		}
		});

	if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
		matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
	}
	return matched_documents;
}

template <typename Policy>
std::vector<Document> SearchServer::FindTopDocuments(const Policy& policy, std::string_view raw_query, DocumentStatus status) const {
	return FindTopDocuments(policy, raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
		return document_status == status;
		});
}

template <typename Policy>
std::vector<Document> SearchServer::FindTopDocuments(const Policy& policy, std::string_view raw_query) const {
	return FindTopDocuments(policy, raw_query, DocumentStatus::ACTUAL);
}


template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const {
	std::map<int, double> document_to_relevance;
	for (std::string_view word : query.plus_words) {
		if (word_to_document_freqs_.count(word) == 0) {
			continue;
		}
		std::string word_str(word);
		const double inverse_document_freq = ComputeWordInverseDocumentFreq(word_str);
		for (const auto& [document_id, term_freq] : word_to_document_freqs_.at(word)) {
			const auto& document_data = documents_.at(document_id);
			if (document_predicate(document_id, document_data.status, document_data.rating)) {
				document_to_relevance[document_id] += term_freq * inverse_document_freq;
			}
		}
	}
	for (std::string_view word : query.minus_words) {
		if (word_to_document_freqs_.count(word) == 0) {
			continue;
		}
		for (const auto& [document_id, _] : word_to_document_freqs_.at(word)) {
			document_to_relevance.erase(document_id);
		}
	}

	std::vector<Document> matched_documents;
	for (const auto& [document_id, relevance] : document_to_relevance) {
		matched_documents.push_back({ document_id, relevance, documents_.at(document_id).rating });
	}
	return matched_documents;
}

template <typename DocumentPredicate, typename Policy>
std::vector<Document> SearchServer::FindAllDocuments(const Policy&, const Query& query, DocumentPredicate document_predicate) const {
	if constexpr (!(std::is_same_v<std::decay_t<Policy>, std::execution::parallel_policy >)) {
		return FindAllDocuments(query, document_predicate);
	}

	ConcurrentMap<int, double> document_to_relevance(NUM_CPUS);
	{

		for_each(
			std::execution::par,
			query.plus_words.begin(), query.plus_words.end(),
			[&, document_predicate](const auto& word) {
				if (word_to_document_freqs_.count(word) != 0) {
					//std::string word_str(word);
					const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
					for (const auto& [document_id, term_freq] : word_to_document_freqs_.at(word)) {
						const auto& document_data = documents_.at(document_id);
						if (document_predicate(document_id, document_data.status, document_data.rating)) {
							document_to_relevance[document_id].ref_to_value += term_freq * inverse_document_freq;
						}
					}
				}
			}
		);
	}
	{

		for_each(
			std::execution::par,
			query.minus_words.begin(), query.minus_words.end(),
			[&](auto word) {if (word_to_document_freqs_.count(word) != 0) {
			for (const auto& [document_id, _] : word_to_document_freqs_.at(word)) {
				document_to_relevance.Delete(document_id);
				//document_to_relevance.erase(document_id);
			}
		}
			}
		);
	}
	{

		std::vector<Document> matched_documents;
		for (const auto& [document_id, relevance] : document_to_relevance.BuildOrdinaryMap()) {
			matched_documents.push_back({ document_id, relevance, documents_.at(document_id).rating });
		}


		return matched_documents;
	}
}



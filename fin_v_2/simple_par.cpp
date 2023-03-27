template <typename DocumentPredicate>
std::vector<Document> SearchServer::FindAllDocuments(const std::execution::parallel_policy, const Query& query, DocumentPredicate document_predicate) const {
    std::map<int, double> document_to_relevance;

    std::for_each(std::execution::par, query.plus_words.begin(), query.plus_words.end(), [&](std::string_view word) {
        if (word_to_document_freqs_.count(word) == 0) {
            return;
        }

        std::string word_str(word);
        const double inverse_document_freq = ComputeWordInverseDocumentFreq(word_str);

        std::for_each(std::execution::par, word_to_document_freqs_.at(word).begin(), word_to_document_freqs_.at(word).end(), [&](const auto& pair) {
            const auto& [document_id, term_freq] = pair;
            const auto& document_data = documents_.at(document_id);
            if (document_predicate(document_id, document_data.status, document_data.rating)) {
                std::scoped_lock lock(document_to_relevance[document_id]); // lock to prevent concurrent map access
                document_to_relevance[document_id] += term_freq * inverse_document_freq;
            }
        });
    });

    std::for_each(std::execution::par, query.minus_words.begin(), query.minus_words.end(), [&](std::string_view word) {
        if (word_to_document_freqs_.count(word) == 0) {
            return;
        }

        std::for_each(std::execution::par, word_to_document_freqs_.at(word).begin(), word_to_document_freqs_.at(word).end(), [&](const auto& pair) {
            const auto& [document_id, _] = pair;
            std::scoped_lock lock(document_to_relevance[document_id]); // lock to prevent concurrent map access
            document_to_relevance.erase(document_id);
        });
    });

    std::vector<Document> matched_documents;
    matched_documents.reserve(document_to_relevance.size());
    std::transform(std::execution::par, document_to_relevance.begin(), document_to_relevance.end(), std::back_inserter(matched_documents), [&](const auto& pair) {
        const auto& [document_id, relevance] = pair;
        return Document{document_id, relevance, documents_.at(document_id).rating};
    });

    return matched_documents;
}

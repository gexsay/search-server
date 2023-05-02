#pragma once
#include <string>
#include <stdexcept>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <numeric>
#include <cmath>

#include "document.h"
#include "string_processing.h"

const int MAX_RESULT_DOCUMENT_COUNT = 5;

const std::map<std::string, double> EMPTY_MAP_FREQ;

class SearchServer {
public:
    SearchServer();

    explicit SearchServer(const std::string& stop_words_);

    template <typename Collection>
    explicit SearchServer(const Collection stop_words_);

    void SetStopWords(const std::string& text);

    const std::map<std::string, double>& GetWordFrequencies(int document_id) const;

    void RemoveDocument(int document_id);
    
    typename std::vector<int>::const_iterator begin() const;

    typename std::vector<int>::const_iterator end() const;

    size_t GetDocumentCount() const;

    void AddDocument(int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& ratings);

    template <typename Predicate>
    std::vector<Document> FindTopDocuments(const std::string& raw_query, Predicate predicate_func) const;

    std::vector<Document> FindTopDocuments(const std::string& raw_query, DocumentStatus status) const;

    std::vector<Document> FindTopDocuments(const std::string& raw_query) const;

    std::tuple<std::vector<std::string>, DocumentStatus> MatchDocument(const std::string& raw_query, int document_id) const;

private:
    struct Query {
        std::set<std::string> plus_words;
        std::set<std::string> minus_words;
    };

    struct DocumentData
    {
        int rating;
        DocumentStatus status;
    };

    std::map<std::string, std::map<int, double>> word_to_documents_freqs;
    std::map<int, std::map<std::string, double>> id_to_words_freqs;
    std::set<std::string> stop_words;
    std::map<int, DocumentData> documents;
    std::vector<int> documents_index;

    static bool IsValidWord(const std::string& word);

    bool IsStopWord(const std::string& word) const;

    static int ComputeAverageRating(const std::vector<int>& ratings, int size);

    std::vector<std::string> SplitIntoWordsNoStop(const std::string& text) const;

    Query ParseQuery(const std::string& text) const;

    template <typename Predicate>
    std::vector<Document> FindAllDocuments(const Query& query_words, Predicate predicate_func) const;
};

template <typename Collection>
SearchServer::SearchServer(const Collection stop_words_) {
    using namespace std;
    stop_words = set(stop_words_.begin(), stop_words_.end());
    if (!all_of(stop_words.begin(), stop_words.end(), [](const auto& word) {return IsValidWord(word); })) {
        throw invalid_argument("Стоп-слово содержит недопустимые символы"s);
    }
}

template <typename Predicate>
std::vector<Document> SearchServer::FindAllDocuments(const Query& query_words, Predicate predicate_func) const {
    std::map<int, double> document_to_relevance;
    for (const auto& plus_word : query_words.plus_words) {
        if (word_to_documents_freqs.count(plus_word)) {
            double documents_with_plus_word = static_cast<double>(word_to_documents_freqs.at(plus_word).size());
            double inverse_document_frequency = log(documents.size() / documents_with_plus_word);
            for (const auto& [id, tf] : word_to_documents_freqs.at(plus_word)) {
                document_to_relevance[id] += inverse_document_frequency * tf;
            }
        }
    }

    for (const auto& minus_word : query_words.minus_words) {
        if (word_to_documents_freqs.count(minus_word)) {
            for (const auto& [id, tf] : word_to_documents_freqs.at(minus_word)) {
                document_to_relevance.erase(id);
            }
        }
    }

    std::vector<Document> matched_documents;
    for (const auto& [id, tfidf] : document_to_relevance) {
        DocumentData data = documents.at(id);
        DocumentStatus status = data.status;
        int rating = data.rating;
        if (predicate_func(id, status, rating)) {
            matched_documents.push_back({ id, tfidf, rating });
        }
    }

    return matched_documents;
}

template <typename Predicate>
std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query, Predicate predicate_func) const {
    Query query_words = ParseQuery(raw_query);
    auto matched_documents = FindAllDocuments(query_words, predicate_func);
    const double EPSILON = 1e-6;
    sort(matched_documents.begin(), matched_documents.end(),
        [&EPSILON](const Document& lhs, const Document& rhs) {
            if (std::abs(lhs.relevance - rhs.relevance) < EPSILON) {
                return lhs.rating > rhs.rating;
            }
    return lhs.relevance > rhs.relevance;
        });

    if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
        matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
    }

    return matched_documents;
}

void RemoveDuplicates(SearchServer& search_server);
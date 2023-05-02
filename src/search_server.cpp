#include "search_server.h"

using namespace std;

SearchServer::SearchServer() : SearchServer(vector<string>{}) {}

SearchServer::SearchServer(const std::string& stop_words_) {
    if (!IsValidWord(stop_words_)) {
        throw invalid_argument("Стоп-слово содержит недопустимые символы"s);
    }
    const vector<string> stop_words_vector = SplitIntoWords(stop_words_);
    stop_words = set(stop_words_vector.begin(), stop_words_vector.end());
}

typename std::vector<int>::const_iterator SearchServer::begin() const {
    return documents_index.cbegin();
}

typename std::vector<int>::const_iterator SearchServer::end() const {
    return documents_index.cend();
}

void SearchServer::SetStopWords(const string& text) {
    for (const string& word : SplitIntoWords(text)) {
        stop_words.insert(word);
    }
}

const map<string, double>& SearchServer::GetWordFrequencies(int document_id) const {
    
    auto value = id_to_words_freqs.lower_bound(document_id);
    if (value != id_to_words_freqs.end()) {
        const map<string, double>& result = (*value).second;
        return result;
    }

    const map<string, double>& empty_result = {};
    return EMPTY_MAP_FREQ;
}

void SearchServer::RemoveDocument(int document_id) {
    documents_index.erase(find(documents_index.begin(), documents_index.end(), document_id));
    documents.erase(document_id);
    id_to_words_freqs.erase(document_id);
    for (auto it = word_to_documents_freqs.begin(); it != word_to_documents_freqs.end(); ++it) {
        auto& id_and_freq = it->second;
        id_and_freq.erase(document_id);
    }
}

size_t SearchServer::GetDocumentCount() const {
    return documents.size();
}

void SearchServer::AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings) {
    if (document_id < 0) {
        throw invalid_argument("Введен отрицательный id"s);
    }
    if (documents.count(document_id) && documents.size()) {
        throw invalid_argument("Документ с введенным id уже существует"s);
    }
    if (!IsValidWord(document)) {
        throw invalid_argument("Слово документа содержит недопустимые символы"s);
    }
    const vector<string> words = SplitIntoWordsNoStop(document);
    int ratings_size = static_cast<int>(ratings.size());
    int average_rating = ComputeAverageRating(ratings, ratings_size);
    const double inv_word_count = 1.0 / words.size();
    for (const string& word : words) {
        word_to_documents_freqs[word][document_id] += inv_word_count;
        id_to_words_freqs[document_id][word] += inv_word_count;
    }
    documents.insert({ document_id, {average_rating, status} });
    documents_index.push_back(document_id);
}

vector<Document> SearchServer::FindTopDocuments(const string& raw_query, DocumentStatus status) const {
    return FindTopDocuments(raw_query, [&status](int document_id, DocumentStatus doc_status, int rating) {return status == doc_status; });
}

vector<Document> SearchServer::FindTopDocuments(const string& raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}

tuple<vector<string>, DocumentStatus> SearchServer::MatchDocument(const string& raw_query, int document_id) const {
    Query query_words = ParseQuery(raw_query);
    DocumentStatus status = documents.at(document_id).status;
    vector<string> match_words;
    for (const string& minus_word : query_words.minus_words) {
        if (word_to_documents_freqs.at(minus_word).count(document_id)) {
            match_words.clear();

            return tuple(match_words, status);
        }
    }

    for (const string& plus_word : query_words.plus_words) {
        if (word_to_documents_freqs.count(plus_word)) {
            if (word_to_documents_freqs.at(plus_word).count(document_id)) {
                match_words.push_back(plus_word);
            }
        }
    }

    return tuple(match_words, status);
}

bool SearchServer::IsValidWord(const string& word) {
    return none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
        });
}

bool SearchServer::IsStopWord(const string& word) const {
    return stop_words.count(word) > 0;
}

int SearchServer::ComputeAverageRating(const vector<int>& ratings, int size) {
    if (ratings.empty()) {
        return 0;
    }

    return accumulate(ratings.begin(), ratings.end(), 0) / size;
}

vector<string> SearchServer::SplitIntoWordsNoStop(const string& text) const {
    vector<string> words;
    for (const string& word : SplitIntoWords(text)) {
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }

    return words;
}

SearchServer::Query SearchServer::ParseQuery(const string& text) const {
    Query query_words;
    for (const string& word : SplitIntoWordsNoStop(text)) {
        if (!IsValidWord(word)) {
            throw invalid_argument("Слово запроса содержит недопустимые символы"s);
        }
        if (word[0] != '-') {
            query_words.plus_words.insert(word);
        }
        else {
            string word_without_minus = word.substr(1);
            if (word_without_minus.empty() || word_without_minus[0] == '-') {
                throw invalid_argument("Недопустимый символ после \"-\"");
            }
            query_words.minus_words.insert(word_without_minus);
        }
    }

    return query_words;
}
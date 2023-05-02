#pragma once
#include <deque>

#include "search_server.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);

    template <typename DocumentPredicate>
    void AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);
    void AddFindRequest(const std::string& raw_query, DocumentStatus status);
    void AddFindRequest(const std::string& raw_query);
    uint64_t GetNoResultRequests() const;

private:
    std::deque<bool> requests;
    const SearchServer& reference_server;
    const static int max_in_day = 1440;
};

template <typename DocumentPredicate>
void RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
    std::vector<Document> results = reference_server.FindTopDocuments(raw_query, document_predicate);
    if (requests.size() >= max_in_day) {
        requests.pop_front();
    }
    requests.push_back(results.empty());
}

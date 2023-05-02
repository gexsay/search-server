#pragma once
#include <string>

#include "request_queue.h"

using namespace std;

RequestQueue::RequestQueue(const SearchServer& search_server) : reference_server(search_server) {}

void RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
    vector<Document> results = reference_server.FindTopDocuments(raw_query, status);
    if (requests.size() >= max_in_day) {
        requests.pop_front();
    }
    requests.push_back(results.empty());
}

void RequestQueue::AddFindRequest(const string& raw_query) {
    vector<Document> results = reference_server.FindTopDocuments(raw_query);
    if (requests.size() >= max_in_day) {
        requests.pop_front();
    }
    requests.push_back(results.empty());
}

uint64_t RequestQueue::GetNoResultRequests() const {
    return count(requests.begin(), requests.end(), true);
}

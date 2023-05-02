#include "remove_duplicates.h"

#include <iostream>
#include <vector>

using namespace std;

set<string> MakeSet(const map<string, double> input) {
    set<string> result;
    for (const auto& [word, _] : input) {
        result.insert(word);
    }

    return result;
}

void RemoveDuplicates(SearchServer& search_server) {
    std::map<std::set<std::string>, int> unique_contents;
    std::vector<int> ids_to_delete;
    for (const auto document_id : search_server) {
        const auto& words_in_document = search_server.GetWordFrequencies(document_id);
        const auto words_set = MakeSet(words_in_document);
        const auto result_iter = unique_contents.find(words_set);
        if (result_iter == unique_contents.end()) {
            unique_contents.insert({ words_set, document_id });
            continue;
        }

        const auto compared_id = result_iter->second;
        if (compared_id < document_id) {
            ids_to_delete.push_back(document_id);
            continue;
        }

        ids_to_delete.push_back(compared_id);
        result_iter->second = document_id;
    }

    for (const auto deleting_id : ids_to_delete) {
        std::cout << "Found duplicate document id "s << deleting_id << std::endl;
        search_server.RemoveDocument(deleting_id);
    }
}
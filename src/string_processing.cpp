#pragma once
#include <vector>
#include <string>

#include "request_queue.h"
#include "string_processing.h"

using namespace std;

vector<std::string> SplitIntoWords(const std::string& text) {
    vector<string> words;
    string word;
    for (const auto element : text) {
        if (element == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += element;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
        word.clear();
    }

    return words;
}
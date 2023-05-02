#pragma once
#include <sstream>
#include <iostream>
#include <vector>

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED
};

struct Document {
    Document();
    Document(int id_, double relevance_, int rating_);

    int id;
    double relevance;
    int rating;
};

std::ostream& operator<< (std::ostream& out, const Document& document);
void PrintDocument(const Document& document);
void PrintMatchDocumentResult(int document_id, const std::vector<std::string>& words, DocumentStatus status);
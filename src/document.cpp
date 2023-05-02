#include "document.h"

std::ostream& operator<< (std::ostream& out, const Document& document) {
    out << "{ document_id = " << document.id << ", relevance = " << document.relevance << ", rating = " << document.rating << " }";
    return out;
}

Document::Document() : id(0), relevance(0), rating(0) {}
Document::Document(int id_, double relevance_, int rating_) : id(id_), relevance(relevance_), rating(rating_) {}

void PrintDocument(const Document& document) {
    using namespace std;
    cout << "{ "s
        << "document_id = "s << document.id << ", "s
        << "relevance = "s << document.relevance << ", "s
        << "rating = "s << document.rating
        << " }"s << endl;
}

void PrintMatchDocumentResult(int document_id, const std::vector<std::string>& words, DocumentStatus status) {
    using namespace std;
    cout << "{ "s
        << "document_id = "s << document_id << ", "s
        << "status = "s << static_cast<int>(status) << ", "s
        << "words ="s;

    for (const string& word : words) {
        cout << ' ' << word;
    }
    std::cout << "}"s << std::endl;
}
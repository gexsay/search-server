#pragma once
#include <sstream>

template <typename Iterator>
class IteratorRange {
public:
    IteratorRange() = default;

    IteratorRange(Iterator begin, Iterator end) : begin_(begin), end_(end) {
        size_ = distance(begin_, end_);
    }

    const Iterator begin() const {
        return begin_;
    }

    const Iterator end() const {
        return end_;
    }

    Iterator begin() {
        return begin_;
    }

    Iterator end() {
        return end_;
    }

    int size() {
        return size_;
    }

private:
    Iterator begin_, end_;
    int size_;
};

template <typename Iterator>
std::ostream& operator<< (std::ostream& out, const IteratorRange<Iterator>& range) {
    for (Iterator currentIt = range.begin(); currentIt < range.end(); ++currentIt) {
        out << *currentIt;
    }
    return out;
}

template <typename Iterator>
class Paginator {
public:
    Paginator() = default;

    Paginator(Iterator begin, Iterator end, size_t page_size) {
        for (auto it = begin; it < end; advance(it, 1)) {
            if (distance(it, end) < page_size) {
                IteratorRange<Iterator> page(it, end);
                pages.push_back(page);
                break;
            }
            else {
                IteratorRange<Iterator> page(it, next(it, page_size));
                pages.push_back(page);
                advance(it, page_size - 1);
            }
        }
    }

    auto size() const {
        return pages.size();
    }

    auto begin() const {
        return pages.begin();
    }

    auto end() const {
        return pages.end();
    }

    auto begin() {
        return pages.begin();
    }

    auto end() {
        return pages.end();
    }

private:
    std::vector<IteratorRange<Iterator>> pages;
};

template <typename Iterator>
std::ostream& operator<<(std::ostream& out, const Paginator<Iterator>& p) {
    for (auto it = p.begin(); distance(it, p.end()) > 0; advance(p)) {
        out << *it;
    }
    return out;
}

template <typename Iterator>
IteratorRange<Iterator>& operator*(const IteratorRange<Iterator>& it) {
    return it;
}

template <typename Iterator>
bool operator!=(const IteratorRange<Iterator>& left, const IteratorRange<Iterator>& right) {
    return left.begin() != right.begin();
}

template <typename Container>
auto Paginate(const Container& c, int page_size) {
    return Paginator(begin(c), end(c), page_size);
}
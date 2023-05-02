#pragma once
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

#include "document.h"

template <typename Container>
void Print(std::ostream& out, const Container& container) {
    using namespace std;
    bool is_first = true;
    for (const auto& element : container) {
        if (!is_first) {
            out << ", "s;
        }
        is_first = false;
        out << element;
    }
}

template <typename Element>
std::ostream& operator<<(std::ostream& out, const std::vector<Element>& container) {
    using namespace std;
    out << "["s;
    Print(out, container);
    out << "]"s;
    return out;
}
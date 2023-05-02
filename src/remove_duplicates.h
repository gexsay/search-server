#pragma once

#include <set>

#include "search_server.h"

std::set<std::string> MakeSet(const std::map<std::string, double> input);
void RemoveDuplicates(SearchServer& search_server);
#ifndef UTILITY_H
#define UTILITY_H

#include <algorithm>
#include <cctype>
#include <string>
#include <utility>
#include <vector>

namespace utility {
void ltrim(std::string &s);
void rtrim(std::string &s);
std::vector<std::string> parseCSV(const std::string &input);
std::pair<std::string, std::string> parseSlash(const std::string &input);
} // namespace utility

#endif
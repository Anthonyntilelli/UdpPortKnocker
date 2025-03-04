#ifndef __UTILITY__
#define __UTILITY__

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <utility>

namespace utility {
    void ltrim(std::string &s);
    void rtrim(std::string &s);
    std::vector<std::string> parseCSV(const std::string &input);
    std::pair<std::string, std::string> parseSlash(const std::string &input);
}

#endif
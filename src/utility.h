#ifndef __UTILITY__
#define __UTILITY__

#include <string>
#include <vector>
#include <cctype>

namespace utility {
    void ltrim(std::string &s);
    void rtrim(std::string &s);
    std::vector<std::string> parseCSV(const std::string &input);
}

#endif
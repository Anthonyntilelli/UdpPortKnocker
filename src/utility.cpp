#ifndef __UTILITY__
#define __UTILITY__

#include <string>
#include <sstream>
#include <vector>

namespace Utility {
    // trim from start (in place)
    inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    // trim from end (in place)
    inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }
    std::vector<std::string> parseCSV(const std::string &input) {
        std::vector<std::string> result;
        std::stringstream ss(input);
        std::string token;
    
        while (std::getline(ss, token, ',')) {
            // Trim spaces
            size_t start = token.find_first_not_of(" \t");
            size_t end = token.find_last_not_of(" \t");
            if (start != std::string::npos && end != std::string::npos) {
                token = token.substr(start, end - start + 1);
            }
            result.push_back(token);
        }
    
        return result;
    }
    
}
#endif
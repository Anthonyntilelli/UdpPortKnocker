#include "utility.h"

namespace utility {
// trim from start (in place)
void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
          }));
}

// trim from end (in place)
void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](unsigned char ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

// create a vector sting list of stings from a CSV line
std::vector<std::string> parseCSV(const std::string &input) {
  std::vector<std::string> parsedVec{};
  std::string str{};
  for (auto c : input) {
    if (std::isspace(c))
      continue; // do nothing on white space.
    if (c == ',' && !str.empty()) {
      parsedVec.push_back(str);
      str.erase();
    } else if (c != ',')
      str += c;
  }
  if (!str.empty())
    parsedVec.push_back(str); // push back last element
  return parsedVec;
}
// splits a "/" into two parts, the second part will be "" if no / is present
std::pair<std::string, std::string> parseSlash(const std::string &input) {
  size_t pos = input.find('/');
  if (pos == std::string::npos)
    return {input, ""}; // No slash
  return {input.substr(0, pos), input.substr(pos + 1)};
}
} // namespace utility

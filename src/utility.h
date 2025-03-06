#ifndef UTILITY_H
#define UTILITY_H

#include <algorithm>
#include <arpa/inet.h>
#include <cctype>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <netinet/in.h>
#include <openssl/sha.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <vector>

namespace utility {
void ltrim(std::string &s);
void rtrim(std::string &s);
std::vector<std::string> parseCSV(const std::string &input);
std::pair<std::string, std::string> parseSlash(const std::string &input);
int stoi(const std::string &str, const std::string &invalid_argument_message,
         const std::string &out_of_range_message);
std::string timeStamp();
std::string makeAuthHash(const int port, const std::string &secret);
bool validateHash(const std::string &hash, const int port,
                  const std::string &secret, const unsigned int leeway);
void knockIp4(const std::string &destinationIp, const unsigned short port,
              const std::string &message);
} // namespace utility

#endif
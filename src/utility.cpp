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
// Note parseCSV will remove whitespace.
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

// Throws invalid_argument messages on failure
int stoi(const std::string &str, const std::string &invalid_argument_message,
         const std::string &out_of_range_message) {
  int number{};
  try {
    number = std::stoi(str);
  } catch (std::invalid_argument const &_) {
    throw std::invalid_argument{invalid_argument_message};
  } catch (std::out_of_range const &_) {
    throw std::invalid_argument{out_of_range_message};
  }
  return number;
}

std::string timeStamp() {
  const std::time_t now = std::time(nullptr);
  const auto *utc_time = std::gmtime(&now); // Convert to UTC (GMT+0)
  const std::string time{
      std::to_string(utc_time->tm_year + 1900) +
      std::to_string(utc_time->tm_mon + 1) + std::to_string(utc_time->tm_mday) +
      std::to_string(utc_time->tm_hour) + std::to_string(utc_time->tm_min)};
  return time;
}

// https://stackoverflow.com/questions/2262386/generate-sha256-with-openssl-and-c
std::string Sha256Hash(const std::string &inputStr) {
  unsigned char hash[SHA256_DIGEST_LENGTH];
  const unsigned char *data = (const unsigned char *)inputStr.c_str();
  SHA256(data, inputStr.size(), hash);
  std::stringstream ss;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
  }
  return ss.str();
}

std::string makeAuthHash(const int port, const std::string &secret) {
  const auto time = timeStamp();
  const auto key = time + std::to_string(port) + secret;
  const auto authHash = Sha256Hash(key);
  return std::string(authHash);
}

// leeway is the number of old auth hashes
bool validateHash(const std::string &hash, const int port,
                  const std::string &secret, const unsigned int leeway) {
  const auto time = timeStamp();
  std::vector<std::string> allowedHashes{};
  for (int i = leeway; i >= 0; i--) {
    auto leewayKey = stoi(time, "validateHash:: time convert fail",
                          "validateHash:: time convert fail") -
                     i;
    allowedHashes.push_back(Sha256Hash(
        (std::to_string(leewayKey) + std::to_string(port) + secret)));
  }
  return std::any_of(allowedHashes.begin(), allowedHashes.end(),
                     [hash](auto x) { return x == hash; });
}

// Data is sent but there is no receiving of data
void knockIp4(const std::string &destinationIp, const unsigned short port,
              const std::string &message) {
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
    throw std::runtime_error("Socket creation failed: " +
                             std::string(strerror(errno)));

  // Filling server information
  struct sockaddr_in servaddr {};
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);

  std::string ip = (destinationIp == "localhost") ? "127.0.0.1" : destinationIp;
  if (inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr) <= 0) {
    close(sockfd);
    throw std::runtime_error("Invalid IP address format: " + destinationIp);
  }

  ssize_t sentBytes =
      sendto(sockfd, message.c_str(), message.size(), MSG_CONFIRM,
             (const struct sockaddr *)&servaddr, sizeof(servaddr));
  if (sentBytes < 0) {
    close(sockfd);
    throw std::runtime_error("Failed to send data: " +
                             std::string(strerror(errno)));
  } else if (static_cast<size_t>(sentBytes) != message.size()) {
    close(sockfd);
    throw std::runtime_error("warning: Not all bytes were sent!");
  }
  close(sockfd);
}

IFirewall &getFwInstance(const firewallType type, Logger &log, bool sudo) {
  switch (type) {
  case (firewallType::invalid):
    throw std::runtime_error{"Error:This is not a valid type."};
    break;
  case (firewallType::mock):
    return MockFirewall::getInstance(log);
    break;
  case (firewallType::ufw):
    return UfwFirewall::getInstance(log, sudo);
    break;
  case (firewallType::iptables):
    throw std::runtime_error{"Error: Not Implemented"};
    break;
  default:
    throw std::runtime_error{"Error: Should not get here."};
    break;
  }
}

// Warning:: This function does not vet the command sent to it.
// Please valdate input BEFORE calling this command.
CommandResult execCommand(const std::string &command) {
  std::vector<char> buffer;
  constexpr size_t CHUNK_SIZE = 4096;
  char tempBuffer[CHUNK_SIZE]; // Temporary buffer for reads
  size_t bytesRead;
  std::string full_command = command + " 2>&1";

  // Open a pipe to execute the command
  FILE *pipe = popen(full_command.c_str(), "r");
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }

  while ((bytesRead = fread(tempBuffer, 1, CHUNK_SIZE, pipe)) > 0) {
    buffer.insert(buffer.end(), tempBuffer,
                  tempBuffer + bytesRead); // Append data
  }

  int status = pclose(pipe);
  int exitCode = WEXITSTATUS(status);

  return {std::string(buffer.begin(), buffer.end()), exitCode};
}
} // namespace utility

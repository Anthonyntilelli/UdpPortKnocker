#ifndef LOGGER_H
#define LOGGER_H

#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unistd.h>

// SINGLETONS instance
class Logger {
private:
  explicit Logger(const std::string &logFilePath);
  ~Logger();
  std::ofstream file;
  std::mutex mtx;
  bool reopenLogFile();
  const std::string filePath;
  pid_t pid;

public:
  static Logger &getInstance(const std::string &logFilePath);
  void log(const std::string &message);

  Logger(const Logger &cpy) = delete;
  Logger &operator=(const Logger &cpy) = delete;
  Logger(Logger &org) = delete;
  Logger &operator=(Logger &&org) = delete;
};
#endif
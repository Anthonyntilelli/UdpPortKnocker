#include "logger.h"

Logger::Logger(const std::string &logFilePath)
    : file{logFilePath, std::ios::app}, filePath{logFilePath} {
  if (!file.is_open())
    throw std::runtime_error("Cannot open the log file at: " + logFilePath +
                             ".");
  log("Server Starting: Logger class is created");
}
Logger::~Logger() {
  if (file.is_open())
    file.close();
}

Logger &Logger::getInstance(const std::string &logFilePath) {
  static Logger instance(logFilePath);
  return instance;
}

void Logger::reopenLogFile() {
  if (!file.is_open()) {
    file.open(filePath, std::ios::app);

    if (!file.is_open()) {
      std::cerr << "ERROR: Unable to reopen log file!" << std::endl;
    }
  }
}

void Logger::log(const std::string &message) {
  std::lock_guard<std::mutex> lck(mtx);
  reopenLogFile();

  if (!file.is_open()) {
    std::cerr << "ERROR: Cannot write to log file. Log file is not open!"
              << std::endl;
    return;
  }

  std::time_t now = std::time(nullptr);
  char buf[20];
  std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
  file << "[" << buf << "] " << message << std::endl;
  file.flush();
}
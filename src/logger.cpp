#include "logger.h"

Logger::Logger(const std::string &logFilePath)
    : file{logFilePath, std::ios::app}, filePath{logFilePath} {
  if (!file.is_open())
    throw std::runtime_error("Cannot open the log file at: " + logFilePath +
                             ".");
  log("Server Starting: Logger class is created");
}

Logger::~Logger() {
  try{
    log("Server Stopping: Logger class is destroyed");
  } catch(...){
    std::cerr << "Error in closing the Logger Class." << std::endl;
  }

  if (file.is_open())
    file.close();
}

Logger &Logger::getInstance(const std::string &logFilePath) {
  static Logger instance(logFilePath);
  return instance;
}

bool Logger::reopenLogFile() {
  if (!file.is_open()) {
    file.open(filePath, std::ios::app);

    if (!file.is_open()) {
      std::cerr << "ERROR: Unable to reopen log file!" << std::endl;
      return false;
    }
  }
  return true;
}

void Logger::log(const std::string &message) {
  std::lock_guard<std::mutex> lck(mtx);
  const std::time_t now = std::time(nullptr);
  char buf[20];
  std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
  const std::string fullMessage = "[" + std::string{buf} + "] " + message;

  if (reopenLogFile()) {
    file << fullMessage << std::endl;
    file.flush();
  } else {
    std::cerr << "ERROR: Could not write log (" << fullMessage << ")"
              << std::endl;
  }
}
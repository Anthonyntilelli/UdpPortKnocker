#include "config.h"
#include "utility.h"
#include <cstring>
// #include <atomic>
// #include <csignal>
// #include "logger.h"

constexpr auto CONFIG_FILE{"config/udpknocker.conf"};

// std::atomic<bool> RUNNING{true};
// void signalHandler(int signum) {
//   std::cout << "\nReceived signal " << signum << ", shutting down
//   gracefully...\n"; RUNNING = false;
// }

// always return false
bool help() {
  std::cerr << "Select on of the valid paramater, `validate` or `knock`"
            << std::endl;
  std::cerr << "validate <path to config>" << std::endl;
  std::cerr << "knock <ipaddress (ipv4)> <service from the config>"
            << std::endl;
  return false;
}

// returns success
bool validate(int argc, char *argv[], Config cfg) {
  if (argc != 3) {
    std::cerr << "Missing config file path or too many parameters provided"
              << std::endl;
    return false;
  }
  try {
    cfg.load(argv[2]); // argv[2] is config file path
  } catch (const std::invalid_argument &e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
  return true;
}

// returns success
bool knock(int argc, char *argv[], Config cfg) {
  if (argc != 4) {
    std::cerr << "Too many argument or to Few" << std::endl;
    return false;
  }
  try {
    cfg.load(CONFIG_FILE);
  } catch (const std::invalid_argument &e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
  auto ip = argv[2];
  auto app = argv[3];
  try {
    for (auto kPort : cfg.getSequences().at(app).getKnockPorts()) {
      auto authHash = utility::makeAuthHash(kPort, cfg.getSecretKey());
      // TODO KNOCK
    }
  } catch (std::out_of_range &_) {
    std::cerr << "Unknown application" << std::endl;
    return false;
  }
  return false;
}

// bool server(int argc, char *argv[], Config cfg) {
//   // TODO load config and validate argc and argv
//   Logger &log = Logger::getInstance(cfg.getLogFile());
//   return false;
// }

int main(int argc, char *argv[]) {
  // std::signal(SIGINT, signalHandler);  // Ctrl+C
  // std::signal(SIGTERM, signalHandler); // Kill command

  Config cfg{}; // load before usage
  int success{false};
  if (argc < 2)
    success = help();
  else if (std::strcmp(argv[1], "validate") == 0)
    success = validate(argc, argv, cfg);
  else if (std::strcmp(argv[1], "knock") == 0)
    success = knock(argc, argv, cfg);
  else {
    std::cerr << "Unknown parameter" << std::endl;
    success = false;
  }

  return !success; // success is 0;
}
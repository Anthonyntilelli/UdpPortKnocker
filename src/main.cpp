#include "config.h"
#include "logger.h"
#include "ifirewall.h"
#include "mockFirewall.h"
#include <atomic>
#include <csignal>
#include <cstring>
#include <chrono>
#include <thread>

constexpr auto CONFIG_FILE{"config/udpknocker.conf"};

std::atomic<bool> RUNNING{true};
void signalHandler(int signum) {
  std::cout << "\nReceived signal " << signum << "Shutting down" << std::endl;
  RUNNING = false;
}

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
    std::cerr << "Too many argument or too few" << std::endl;
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
      try {
        std::cout << "Knocking on port: " << kPort << std::endl;
        utility::knockIp4(ip, static_cast<uint16_t>(kPort), authHash);
      } catch (const std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        return false;
      }
      // Short delay before sending each knock
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  } catch (const std::out_of_range &_) {
    std::cerr << "Unknown application (not referenced in config "
                 "<application>_sequence)"
              << std::endl;
    return false;
  }
  return true;
}

bool server(int argc, char *argv[], Config cfg) {
  if (argc != 2) {
    std::cerr << "Too many argument or too few" << std::endl;
    return false;
  }
  try {
    cfg.load(CONFIG_FILE);
  } catch (const std::invalid_argument &e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
  Logger &log = Logger::getInstance(cfg.getLogFile());
  try{
    IFirewall &firewall= utility::getFwInstance(cfg.getFirewall(), log);
  } catch (const std::runtime_error &e){
    std::cerr << e.what() << std::endl;
    return false;
    // TODO:: SERVER
  }
  return false;
}

int main(int argc, char *argv[]) {
  std::signal(SIGINT, signalHandler);  // Ctrl+C
  std::signal(SIGTERM, signalHandler); // Kill command

  Config cfg{}; // load before usage
  int success{false};
  if (argc < 2)
    success = help();
  else if (std::strcmp(argv[1], "validate") == 0)
    success = validate(argc, argv, cfg);
  else if (std::strcmp(argv[1], "knock") == 0)
    success = knock(argc, argv, cfg);
  else if (std::strcmp(argv[1], "server") == 0)
    success = server(argc, argv, cfg);
  else {
    std::cerr << "Unknown parameter" << std::endl;
    success = false;
  }

  return !success; // success is 0;
}
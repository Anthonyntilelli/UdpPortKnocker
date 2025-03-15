#include "config.h"
#include "firewall/ifirewall.h"
#include "logger.h"
#include "udpServer.h"
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstring>
#include <sys/eventfd.h>
#include <thread>
#include <unordered_map>
#include <vector>

int warnFd{-1}; // Global eventfd file descriptor
constexpr auto CONFIG_FILE{"config/udpknocker.conf"};
constexpr unsigned short ALLOWMINUTES{3};
std::atomic<bool> RUNNING{true};

void signalHandler(int signum);
bool help();
bool validate(int argc, char *argv[], Config &cfg);
bool knock(int argc, char *argv[], Config &cfg);
bool server(int argc, Config &cfg);
void serverLoop(const Config &c, Logger &l, IFirewall &f,
                const std::string &application);

int main(int argc, char *argv[]) {
  Config cfg{}; // load before usage
  int success{false};
  if (argc < 2)
    success = help();
  else if (std::strcmp(argv[1], "validate") == 0)
    success = validate(argc, argv, cfg);
  else if (std::strcmp(argv[1], "knock") == 0)
    success = knock(argc, argv, cfg);
  else if (std::strcmp(argv[1], "server") == 0)
    success = server(argc, cfg);
  else {
    std::cerr << "Unknown parameter" << std::endl;
    success = false;
  }

  return !success; // success is 0;
}

void signalHandler(int signum) {
  if (!RUNNING)
    std::exit(EXIT_FAILURE);
  std::cout << "\nReceived signal " << signum << " Shutting down" << std::endl;
  RUNNING = false;

  // unblock the epoll waits
  uint64_t signal_value = 99; // Signal all workers

  auto written = write(warnFd, &signal_value, sizeof(signal_value));
  if (written == -1) {
    std::cerr << "Stop write failed" + std::string(strerror(errno));
  }
}

bool help() {
  std::cerr
      << "Select on of the valid paramater: `validate`, `knock`, or `server` \n"
      << "validate <path to config> \n"
      << "knock <ipaddress (ipv4)> <service from the config> \n"
      << "server" << std::endl;
  return false;
}

bool validate(int argc, char *argv[], Config &cfg) {
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

bool knock(int argc, char *argv[], Config &cfg) {
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

bool server(int argc, Config &cfg) {
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
  try {
    Logger &log = Logger::getInstance(cfg.getLogFile());
    IFirewall &firewall =
        utility::getFwInstance(cfg.getFirewall(), log, cfg.getSudo());

    warnFd = eventfd(0, EFD_NONBLOCK);
    std::vector<std::jthread> threads;
    for (const auto &[app, seq] : cfg.getSequences()) {
      threads.emplace_back(serverLoop, std::ref(cfg), std::ref(log),
                           std::ref(firewall), app);
    }

  } catch (const std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
  return true;
}

void serverLoop(const Config &c, Logger &l, IFirewall &f,
                const std::string &application) {
  std::signal(SIGINT, signalHandler);  // Ctrl+C
  std::signal(SIGTERM, signalHandler); // Kill command
  const std::vector<uint16_t> kPorts =
      c.getSequences().at(application).getKnockPorts();
  const uint16_t unlockport = c.getSequences().at(application).getUnlockPort();
  const Protocol unlockProto =
      (c.getSequences().at(application).isTcp() ? Protocol::tcp
                                                : Protocol::udp);
  std::string zeroIp = "0.0.0.0";
  std::unordered_map<std::string, std::vector<uint16_t>>
      knockTracker{}; // <ipaddress, ports>
  std::unordered_map<std::string, bool>
      firewallTracker{}; //<ipaddress, FirewallOpen>

  // open Knocker Ports
  for (const auto port : kPorts) {
    FirewallRule fr = {zeroIp, Protocol::udp, port};
    if (!f.allow_in(fr.ip, fr.protocol, fr.port))
      l.log("Firewall rule Failed: " + fr.ip + ":" + std::to_string(fr.port));
  }

  auto server = UdpServer{kPorts, warnFd};
  l.log("Server for " + application + " is now listening.");
  while (RUNNING) {
    try {
      auto messages = server.receive();
      for (auto m : messages) {
        if (utility::validateHash(m.message, m.port, c.getSecretKey(),
                                  ALLOWMINUTES)) {
          l.log("Knock received from " + m.ipaddress +
                " on port: " + std::to_string(m.port) + " with an valid hash");
          knockTracker.try_emplace(
              m.ipaddress, std::vector<uint16_t>{}); // Does nothing on already
                                                     // init ip addresses.
          knockTracker[m.ipaddress].push_back(m.port);
          if (knockTracker.at(m.ipaddress).size() >= kPorts.size()) {
            if (knockTracker.at(m.ipaddress) == kPorts) {
              firewallTracker.try_emplace(m.ipaddress, false);

              if (firewallTracker.at(m.ipaddress)) { // firewall rule exists
                f.removeRule(m.ipaddress, unlockProto, unlockport);
                firewallTracker.erase(m.ipaddress);
              } else {
                f.allow_in(m.ipaddress, unlockProto, unlockport);
                firewallTracker[m.ipaddress] = true;
              }
            } else {
              l.log("Warning: Knock received from " + m.ipaddress +
                    " are out of order.");
            }
            knockTracker.erase(m.ipaddress); // clear the tracker
          }
        } else {
          knockTracker.erase(m.ipaddress); // clear the incorrect knock
          // TODO:: BAN?
          l.log("Knock received from " + m.ipaddress +
                " on port: " + std::to_string(m.port) + "with an invalid hash");
        }
      }
    } catch (std::logic_error &e) {
      RUNNING = false;
      l.log(std::string{"Warning: "} + e.what());
    }
  }

  for (const auto &[ip, active] : firewallTracker) {
    if (active) {
      if (!f.removeRule(ip, unlockProto, unlockport))
        l.log(std::string("Firewall rule removal Failed: ") + ip + ":" +
              std::to_string(unlockport));
    }
  }

  for (const auto port : kPorts) {
    if (!f.removeRule(zeroIp, Protocol::udp, port))
      l.log(std::string("Firewall rule removal Failed: ") +
            std::string("0.0.0.0:") + std::to_string(port));
  }
}
#include "config.h"
#include "firewall/ifirewall.h"
#include "logger.h"
#include "udpServer.h"
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstring>
#include <thread>
#include <vector>

constexpr auto CONFIG_FILE{"config/udpknocker.conf"};
std::atomic<bool> RUNNING{true};

void signalHandler(int signum);
bool help();
bool validate(int argc, char *argv[], Config cfg);
bool knock(int argc, char *argv[], Config cfg);
bool server(int argc, Config cfg);
void serverLoop(const Config &c, Logger &l, IFirewall &f, const std::string &application);

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
}

bool help() {
  std::cerr
      << "Select on of the valid paramater: `validate`, `knock`, or `server` \n"
      << "validate <path to config> \n"
      << "knock <ipaddress (ipv4)> <service from the config> \n"
      << "server" << std::endl;
  return false;
}

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

bool server(int argc, Config cfg) {
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
    
    auto app = std::string{"ssh"}; //PLaceholder
    serverLoop(cfg, log,firewall, app);

  } catch (const std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
  return false;
}

void serverLoop(const Config &c, Logger &l, IFirewall &f, const std::string &application){
  std::signal(SIGINT, signalHandler);  // Ctrl+C
  std::signal(SIGTERM, signalHandler); // Kill command

  std::vector<FirewallRule>  frVec{};
  const std::vector<uint16_t> kPorts = c.getSequences().at(application).getKnockPorts();
  const uint16_t unlockport = c.getSequences().at(application).getUnlockPort();
  //open Knocker Ports
  for(const auto port : kPorts){
    FirewallRule fr = {std::string{"0.0.0.0"}, Protocol::udp, port};
    if (f.allow_in(fr.ip, fr.protocol, fr.port)){
      frVec.push_back(fr);
    } else {
      //TODO
      std::cerr << "Firewall rule Failed: " << fr.ip << ":" << fr.port << std::endl;   
    }
  } 
  auto server = UdpServer{kPorts};
  l.log("Server is for " + application + " is now listening.");
  while(RUNNING){
    auto messages = server.receive();
    for (auto m: messages){
      if(utility::validateHash(m.message, m.port,c.getSecretKey(),3)) std::cout << "Valid Message" << std::endl;
      else std::cout << "Invalid Message" << std::endl;
    }
  }
  l.log("Shutting down firewall rules");
  for(auto rule: frVec){
    if(!f.removeRule(rule.ip, rule.protocol, rule.port)){
      //TODO
      std::cerr << "Firewall rule removal Failed: " << rule.ip << ":" << rule.port << std::endl;  
    }
  }



}
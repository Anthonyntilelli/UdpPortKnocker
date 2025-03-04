#include "sequence.h"

Sequence::Sequence() : tcp{false}, unlockPort{-1}, knockPorts{} {}

void Sequence::clearKnockerPorts() { knockPorts.clear(); }

void Sequence::addPortToSequence(const int port) {
  if (port >= 1024 && port <= 65535)
    knockPorts.push_back(port);
  else
    throw std::invalid_argument(
        "Sequence port is not a valid, it must be in between 1024 and 65535");
}

void Sequence::setunlockPort(const int port, const bool isTcp) {
  tcp = isTcp;
  if (port >= 1 && port <= 65535)
    unlockPort = port;
  else
    throw std::invalid_argument(
        "Unlock port is not a valid, it must be in between 1 and 65535");
}

bool Sequence::isValid() const {
  if (unlockPort == -1)
    return false;
  if (knockPorts.empty())
    return false;
  if (knockPorts.size() < 3)
    return false;
  std::unordered_set<int> dupCheck;
  for (auto port : knockPorts) {
    if (dupCheck.find(port) != dupCheck.end())
      return false; // duplicate port found
    dupCheck.insert(port);
  }
  // unlock port is one of the knocker ports
  if (dupCheck.find(unlockPort) != dupCheck.end() && !tcp)
    return false;
  return true;
}

int Sequence::getUnlockPort() const { return unlockPort; }

bool Sequence::isTcp() const { return tcp; }

std::vector<int> Sequence::getKnockPorts() const { return knockPorts; }

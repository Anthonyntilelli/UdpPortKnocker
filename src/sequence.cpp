#include "sequence.h"

Sequence::Sequence() : tcp{false}, unlockPort{0}, knockPorts{} {}

void Sequence::clearKnockerPorts() { knockPorts.clear(); }

// Port is int because it needs to handle potentially ivalid numbers
void Sequence::addPortToSequence(const int port) {
  if (port >= 1024 && port <= 65535)
    knockPorts.push_back(static_cast<uint16_t>(port));
  else
    throw std::invalid_argument(
        "Sequence port is not a valid, it must be in between 1024 and 65535");
}

// Port is int because it needs to handle potentially ivalid numbers
void Sequence::setUnlockPort(const int port, const bool isTcp) {
  tcp = isTcp;
  if (port >= 1 && port <= 65535)
    unlockPort = static_cast<uint16_t>(port);
  else
    throw std::invalid_argument(
        "Unlock port is not a valid, it must be in between 1 and 65535");
}

bool Sequence::isValid() const {
  if (unlockPort == 0)
    return false;
  if (knockPorts.empty())
    return false;
  if (knockPorts.size() < 3)
    return false;
  std::unordered_set<uint16_t> dupCheck;
  for (auto port : knockPorts) {
    if (dupCheck.find(port) != dupCheck.end())
      return false; // duplicate port found
    dupCheck.insert(port);
  }
  // unlock port is not one of the knocker ports
  if (dupCheck.find(unlockPort) != dupCheck.end() && !tcp)
    return false;
  return true;
}

uint16_t Sequence::getUnlockPort() const { return unlockPort; }

bool Sequence::isTcp() const { return tcp; }

std::vector<uint16_t> Sequence::getKnockPorts() const { return knockPorts; }

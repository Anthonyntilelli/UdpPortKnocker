#ifndef SEQUENCE_H
#define SEQUENCE_H
#include <stdexcept>
#include <unordered_set>
#include <vector>

class Sequence {
private:
  bool tcp; // false is udp
  uint16_t unlockPort;
  std::vector<uint16_t> knockPorts;

public:
  // Check if the port is in range of actual ports and pushes back if valid
  // Throws std::invalid_argument if invalid
  Sequence();
  void clearKnockerPorts();
  void addPortToSequence(const int port);
  // Check is the unlock port is in a range of allowed ports
  // Throws std::invalid of not in range
  void setUnlockPort(const int port, const bool isTcp);
  // returns if sequence is valid
  bool isValid() const;
  uint16_t getUnlockPort() const;
  bool isTcp() const;
  std::vector<uint16_t> getKnockPorts() const;
  ~Sequence() = default;
};

#endif
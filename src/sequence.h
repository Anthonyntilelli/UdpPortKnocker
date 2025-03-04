#ifndef __SEQUENCE__
#define __SEQUENCE__
#include <stdexcept>
#include <unordered_set>
#include <vector>

class Sequence {
private:
  bool tcp; // false is udp
  int unlockPort;
  std::vector<int> knockPorts;

public:
  // Check if the port is in range of actual ports and pushes back if valid
  // Throws std::invalid_argument if invalid
  Sequence();
  void clearKnockerPorts();
  void addPortToSequence(const int port);
  // Check is the unlock port is in a range of allowed ports
  // Throws std::invalid of not in range
  void setunlockPort(const int port, const bool isTcp);
  // returns if sequence is valid
  bool isValid() const;
  int getUnlockPort() const;
  bool isTcp() const;
  std::vector<int> getKnockPorts() const;
  ~Sequence() = default;
};

#endif
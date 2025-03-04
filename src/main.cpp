#include "config.h"

constexpr auto config_file {"config/udpknocker.conf"};

int main () {
    Config cfg(config_file);
    return 0;
}
#include "config.h"
#include "utility.h"

constexpr auto config_file {"config/udpknocker.conf"};

int main () {
    Config cfg(config_file);
    return 0;
}
# UdpPortKnocker - PortKnocker built in C++

## Table of Contents

1. [Description](#Description)
2. [Parameters](#Parameters)
3. [Examples](#Examples)
4. [Build](#Build)
5. [Mic](#Mic)
6. [Authors](#Authors)

## Description <a name= "Description"></a>

This application is a variant of a port knocker and server that listen to key hashed on udp ports. To open a port, knock on the ports with the correct hash and the firewall will open a port. To close the port, simply close knock again on those same ports.

This port knocker only add allow in rule to the firewall, it assume that the is a default deny/reject input rule in place. The firewall work with ufw and iptables. There is a mock mode to see what the program would do to the firewall, without making any actual changes. The program will remove all allow rule when it is closed.

To close the server simply press control+c and wait a second.  Pressing control + c again will kill the program, which may leave behind some firewall rules.

The config for the program must be in "./config/udpknocker.conf"

## Parameters: <a name="Parameters"></a>

`udpknocker.out` - prints the help menu

`udpknocker.out validate <CONFIG>` - validate a config is correct.

    CONFIG - Path to the config file to test

`udpknocker.out knock <IP> <APPLICATION>` - preforms the knock sequence.

    IP - an IPv4 address you want to knock on.
    APPLICATION - name of the application as its found in the config.
        e.g. ssh_unlock/ssh_sequence is the "ssh" application

`udpknocker.out server` - Listen for knocks

## Examples: <a name="Examples"></a>

`udpknocker.out validate /config/udpknocker.conf`

`udpknocker.out knock localhost ssh`

`udpknocker.out server`

## Build <a name="Build"></a>

Application is not compatible with windows it relies on linux specific apis

### Dependencies
- g++ 11.4.0
- `sudo apt install libssl-dev` (Pop!_OS 22.04 LTS)

### Compile

`g++ -std=c++23 src/*.cpp src/firewall/*.cpp -lssl -lcrypto -o bin/udpknocker.out`

### Testing build

`g++ -fdiagnostics-color=always -fno-omit-frame-pointer -Og -Wall -Wextra -Wpedantic -Wshadow -Wconversion -std=c++23 -fsanitize=address,undefined,leak -D_GLIBCXX_ASSERTIONS src/*.cpp src/firewall/*.cpp  -lssl -lcrypto -o bin/udpknocker.out`

## Mic <a name="Mic"></a>

### Manual Build (No ASan)
`g++ -fdiagnostics-color=always -fno-omit-frame-pointer -Og -Wall -Wextra -Wpedantic -Wshadow -Wconversion -std=c++23  -D_GLIBCXX_ASSERTIONS src/*.cpp src/firewall/*.cpp -lssl -lcrypto -o bin/udpknocker.out`

### Leak testing

`valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./bin/udpknocker.out # Memory checks`
`valgrind --tool=helgrind ./bin/udpknocker.out # thread test(s)`

### Static Analysis

`cppcheck --enable=all --check-config --inconclusive --suppress=missingIncludeSystem src/`

### Formatting
`find src -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" \) -exec clang-format -i {} \;`

## Authors: <a name="Authors"></a>

- Anthony Tilelli

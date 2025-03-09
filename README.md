# UdpPortKnocker
A port knocker server written is C++

The firewall for this application only modifies input related rules, it will not touch the outgoing rules.

## Manual BUILD (TESTING)

`g++ -fdiagnostics-color=always -fno-omit-frame-pointer -Og -Wall -Wextra -Wpedantic -Wshadow -Wconversion -std=c++23 -fsanitize=address,undefined,leak -D_GLIBCXX_ASSERTIONS src/*.cpp -lssl -lcrypto -o bin/udpknocker.out`

`./bin/udpknocker.out`

 ## Manual Build (No ASan)
`g++ -fdiagnostics-color=always -fno-omit-frame-pointer -Og -Wall -Wextra -Wpedantic -Wshadow -Wconversion -std=c++23  -D_GLIBCXX_ASSERTIONS src/*.cpp -lssl -lcrypto -o bin/udpknocker.out`

## Leak testing

`valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./bin/udpknocker.out # Memory checks`

`valgrind --tool=helgrind ./bin/udpknocker.out # thread test(s)`


## Static Analysis

`cppcheck --enable=all --check-config --inconclusive --suppress=missingIncludeSystem src/`

## Formatting
`clang-format -i src/*`

## Dependencies
- `sudo apt install libssl-dev`


`g++ -std=c++23 src/*.cpp -lssl -lcrypto -o bin/udpknocker.out`

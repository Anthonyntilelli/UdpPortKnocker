# UdpPortKnocker
A port knocker server written is C++

## Manual BUILD (TESTING)

`g++ -fdiagnostics-color=always -fno-omit-frame-pointer -Og -Wall -Wextra -Wpedantic -Wshadow -Wconversion -std=c++23 -fsanitize=address,undefined,leak -D_GLIBCXX_ASSERTIONS src/*.cpp -o bin/udpknocker.out`

no ASan
`g++ -fdiagnostics-color=always -fno-omit-frame-pointer -Og -Wall -Wextra -Wpedantic -Wshadow -Wconversion -std=c++23 -D_GLIBCXX_ASSERTIONS src/*.cpp -o bin/udpknocker.out`

valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./bin/udpknocker.out

## Static Analysis

`cppcheck --enable=all --inconclusive src/`


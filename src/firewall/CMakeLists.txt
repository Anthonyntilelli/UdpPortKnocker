message("In Firewall Library")

set(SOURCE_FILES iptableFirewall.cpp mockFirewall.cpp ufwFirewall.cpp)

add_library(Firewall STATIC ${SOURCE_FILES})
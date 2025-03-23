message("In Source Files")

set(SOURCE_FILES config.cpp logger.cpp main.cpp sequence.cpp udpServer.cpp utility.cpp)

add_library(Source STATIC ${SOURCE_FILES})
add_subdirectory(firewall)
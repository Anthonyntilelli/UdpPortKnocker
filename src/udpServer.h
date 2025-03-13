#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/epoll.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <array>
#include <string>
#include <map>
#include <vector>

struct message {
    std::string message;
    uint16_t port;
    std::string ipaddress;
};

// IP V4 only
class UdpServer {
    private:
    static constexpr size_t BUFFERSIZE = 4096;
    static constexpr size_t MAXEVENTS = 10;
    
    //file descriptor and port
    std::map<int,uint16_t> sockfds;
    int epoll_fd;
    bool socketsOpen;

    int makeUdpPort(uint16_t port) const;
    void closeAllSocketFd();

    public:
    UdpServer(std::vector<uint16_t> ports);
    ~UdpServer();
    std::vector<message> receive();
};
#endif

#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <bits/stdc++.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <string>

struct message {
    std::string message;
    size_t port;
};

// IP V4 only
class UdpServer {
    private:

    const int BUFFERSIZE = 4096;
    int sockfd;
    size_t port;
    sockaddr_in servaddr; 

    public:
    UdpServer(size_t UDPport);
    ~UdpServer();
    message receive();
};
#endif

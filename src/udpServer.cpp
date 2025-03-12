#include "udpServer.h"

UdpServer::UdpServer(size_t UDPport): sockfd(), port{UDPport}, servaddr{} {
  if (!(port >= 1 && port <= 65535)) throw std::runtime_error("Port is not in correct range.");
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) throw std::runtime_error("Socket creation failed: " + std::string(strerror(errno)));

  // Server settings
  servaddr.sin_family    = AF_INET; // IPv4 
  servaddr.sin_addr.s_addr = INADDR_ANY; 
  servaddr.sin_port = htons(static_cast<uint16_t>(port)); 

  if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) throw std::runtime_error("Bind failed: " + std::string(strerror(errno)));

}

UdpServer::~UdpServer() {
  close(sockfd);
}

message UdpServer::receive() { 
  std::vector<char> buffer(BUFFERSIZE);; 
  sockaddr_in cliaddr {};
  
  socklen_t len = sizeof(cliaddr);   
  int n = recvfrom(sockfd, buffer.data(), BUFFERSIZE, 0, (struct sockaddr *)&cliaddr, &len);;
  if (n < 0) throw std::runtime_error("receive from error: " + std::string(strerror(errno))); 

  return {std::string{buffer.data()}, port}; 
}

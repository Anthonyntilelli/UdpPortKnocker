#include "udpServer.h"

// Makes a udp socket and binds it to the port;
int UdpServer::makeUdpPort(const uint16_t port) const {
  int sockfd{};
  struct sockaddr_in addr {};

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    throw std::runtime_error("Socket creation failed: " +
                             std::string(strerror(errno)));

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    close(sockfd);
    throw std::runtime_error("Socket binding to port failed: " +
                             std::string(strerror(errno)));
  }

  return sockfd;
}

void UdpServer::closeAllSocketFd() {
  if (socketsOpen) {
    for (auto [key, _] : sockfds) {
      close(key);
    }
    socketsOpen = false;
  }
}

UdpServer::UdpServer(std::vector<uint16_t> ports, int wakefd)
    : sockfds{}, epoll_fd{}, socketsOpen{true}, centralfd(wakefd) {

  epoll_fd = epoll_create(10);
  if (epoll_fd == -1)
    throw std::runtime_error("epoll socket creation failed: " +
                             std::string(strerror(errno)));

  for (auto port : ports) {
    int fd = makeUdpPort(port);
    sockfds[fd] = port;

    struct epoll_event ev;
    ev.events = EPOLLIN; // Interested in readability
    ev.data.fd = fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
      closeAllSocketFd();
      throw std::runtime_error("epoll_ctl failed: " +
                               std::string(strerror(errno)));
    }
  }

  // Adding warning fd
  struct epoll_event ev;
  ev.events = EPOLLIN; // Interested in readability
  ev.data.fd = centralfd;

  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, centralfd, &ev) == -1) {
    closeAllSocketFd();
    throw std::runtime_error("epoll_ctl centralfd failed: " +
                             std::string(strerror(errno)));
  }
}

UdpServer::~UdpServer() { closeAllSocketFd(); }

std::vector<message> UdpServer::receive() {
  std::array<epoll_event, MAXEVENTS> events{};
  std::array<char, BUFFERSIZE> buffer{};
  std::vector<message> messages{};

  int nfds = epoll_wait(epoll_fd, events.data(), events.size(), -1);
  if (nfds == -1 && errno == EINTR) {
    throw std::logic_error("epoll_wait interrupted by signal");
  }
  if (nfds == -1) {
    closeAllSocketFd();
    throw std::runtime_error("epoll_wait failed: " +
                             std::string(strerror(errno)));
  }

  messages.reserve(nfds);
  // Process each ready file descriptor
  for (int i = 0; i < nfds; i++) {
    if (events[i].events & EPOLLIN) {
      int fd = events[i].data.fd;
      struct sockaddr_in client_addr {};
      socklen_t addrlen = sizeof(client_addr);
      std::fill(buffer.begin(), buffer.end(), 0);
      // Dont need to read the wake up event
      if (fd == centralfd)
        continue;
      ssize_t count = recvfrom(fd, buffer.data(), buffer.size() - 1, 0,
                               (struct sockaddr *)&client_addr, &addrlen);
      if (count == -1) {
        std::cerr << "recvfrom failed: " + std::string(strerror(errno))
                  << std::endl;
        continue;
      }
      message m = {buffer.data(), sockfds.at(fd),
                   std::string(inet_ntoa(client_addr.sin_addr))};
      messages.push_back(std::move(m));
    }
  }
  return messages;
}

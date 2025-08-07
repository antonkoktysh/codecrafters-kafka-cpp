#include "Socket.h"

Socket::Socket(int domain, int type, int protocol) {
    fd_ = socket(domain, type, protocol);
    if (fd_ < 0) {
        throw SocketCreationError();
    }
}

Socket::~Socket() {
    if (fd_ >= 0) {
        close(fd_);
    }
}

void Socket::Bind(sockaddr_in& address) {
    if (::bind(fd_, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) != 0) {
        throw BindError();
    }
}
void Socket::Listen(int backlog) {
    if (::listen(fd_, backlog) != 0) {
        throw ListenError();
    }
}
int Socket::Accept(sockaddr_in& client_addr) {
    socklen_t addr_len = sizeof(client_addr);
    int client_fd = ::accept(fd_, reinterpret_cast<struct sockaddr*>(&client_addr), &addr_len);
    if (client_fd < 0) {
        throw AcceptError();
    }
    return client_fd;
}

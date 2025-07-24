#pragma once
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "client_connection.h"

class Socket {
private:
    int fd_ = -1;

public:
    Socket(int domain, int type, int protocol = 0) {
        fd_ = socket(domain, type, protocol);
        if (fd_ < 0) {
            throw SocketCreationError();
        }
    }

    // TODO
    // check unsuccesfull completion (if close return -1)
    ~Socket() {
        if (fd_ >= 0) {
            close(fd_);
        }
    }

    void bind(sockaddr_in& address) {
        if (::bind(fd_, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) != 0) {
            throw BindError();
        }
    }

    void listen(int backlog) {
        if (::listen(fd_, backlog) != 0) {
            throw ListenError();
        }
    }
    int accept(sockaddr_in& client_addr) {
        socklen_t addr_len = sizeof(client_addr);
        int client_fd = ::accept(fd_, reinterpret_cast<struct sockaddr*>(&client_addr), &addr_len);
        if (client_fd < 0) {
            throw AcceptError();
        }
        return client_fd;
    }

    operator int() const {
        return fd_;
    }
};

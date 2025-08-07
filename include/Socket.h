#pragma once
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "server_exceptions.h"
#include "client_connection.h"

class Socket {
private:
    int fd_ = -1;

public:
    Socket(int domain, int type, int protocol = 0);

    Socket() = default;
    ~Socket();

    void Bind(sockaddr_in& address);

    void Listen(int backlog);
    int Accept(sockaddr_in& client_addr);

    operator int() const {
        return fd_;
    }
};



#pragma once
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "server_exceptions.h"
#include "api_reader_factory.h"
class ClientConnection {
private:
    int fd_;
    std::atomic<bool> is_active_ = true;

public:
    ClientConnection(int fd);
    ~ClientConnection();

    std::vector<std::byte> read_full();

    void write_full(std::vector<std::byte>& data);
    void write(std::vector<std::byte>& data, size_t size);

    operator int() const {
        return fd_;
    }
    bool IsActive() const;
};


 void HandleClient(std::unique_ptr<ClientConnection> client);

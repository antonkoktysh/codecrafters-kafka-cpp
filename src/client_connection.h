#pragma once
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <iostream>
#include <stdexcept>
#include <system_error>
#include <vector>
#include "server_exceptions.h"
#include "api_reader_factory.h"
class ClientConnection {
private:
    int fd_;
    std::atomic<bool> is_active_ = true;

public:
    explicit ClientConnection(int fd) : fd_(fd) {
    }
    ~ClientConnection() {
        if (fd_ >= 0) {
            close(fd_);
        }
    }

    std::vector<std::byte> read_full() {
        std::vector<std::byte> buffer(1024);
        ssize_t n = recv(fd_, buffer.data(), buffer.size() * sizeof(std::byte), 0);

        if (n == 0) {
            is_active_ = false;
            throw ClientDisconnectedError();
        }
        if (n < 0) {
            is_active_ = false;
            throw ClientReceiveError();
        }

        buffer.resize(n);
        return buffer;
    }
    // TODO write

    void write_full(std::vector<std::byte>& data, size_t size) {
        if (write(fd_, data.data(), size) < 0) {
            throw std::runtime_error("Error occured during writing");
        }
    }

    operator int() const {
        return fd_;
    }
    bool IsActive() const {
        return is_active_;
    }
};

void HandleClient(std::unique_ptr<ClientConnection> client) {
    while (client->IsActive()) {
        try {
            auto buffer = client->read_full();
            ApiReaderFactory factory;
            auto reader = factory.Create(buffer);
            auto request = reader->ParseRequest();
            auto response = request->Process();
            auto to_send = response->ByteResponse();
            client->write_full(to_send, to_send.size());
        } catch (ClientDisconnectedError& e) {
            // OnClientDisconnected();
            std::cout << e.what() << std::endl;
            break;
        }
    }
}

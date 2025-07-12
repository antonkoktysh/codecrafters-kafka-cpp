#pragma once
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <iostream>
#include <stdexcept>
#include <system_error>
#include <vector>

#include "KafkaResponse.h"
#include "ServerExceptions.h"
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

    std::vector<char> read_full() {
        std::vector<char> buffer(1024);
        ssize_t n = recv(fd_, buffer.data(), buffer.size(), 0);
        if (n == 0) {
            throw ClientDisconnectedError();
            is_active_ = false;
        }
        if (n < 0) {
            is_active_ = false;
            throw ClientReceiveError();
        }

        buffer.resize(n);
        return buffer;
    }
    void write_full(std::vector<char>& data, size_t size) {
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

// https://www.youtube.com/watch?v=xGDLkt-jBJ4&t=2659s
// NB: smart_ptr pass by value
void HandleClient(std::unique_ptr<ClientConnection> client) {
    while (client->IsActive()) {
        try {
            std::cout << "Client connected" << std::endl;
            auto buffer = client->read_full();
            int16_t api_key;
            memcpy(&api_key, buffer.data() + 4, sizeof(api_key));
            std::cout << ntohs(api_key) << std::endl;
            std::unique_ptr<IRequestHandler> response_handler;
            if (ntohs(api_key) == 18) {
                response_handler = std::make_unique<ApiVersionsHandler>(buffer);
            } else {
                response_handler = std::make_unique<DescribeTopicPartitionsHandler>(buffer);
            }
            auto response_buffer = response_handler->GetResponseBuffer();
            size_t response_size = response_handler->ResponseSize();
            client->write_full(response_buffer, response_size);
            std::cout << "Client handled" << std::endl;
        } catch (ClientDisconnectedError& e) {
            std::cout << e.what() << std::endl;
        }
    }
}

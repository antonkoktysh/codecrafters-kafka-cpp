#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "KafkaResponse.h"
class Socket {
    int fd_ = -1;

public:
    Socket(int domain, int type, int protocol = 0) {
        fd_ = socket(domain, type, protocol);
        if (fd_ < 0) {
            throw std::system_error(errno, std::system_category(), "socket creation failed");
        }
    }

    ~Socket() {
        if (fd_ >= 0)
            close(fd_);
    }

    void bind(sockaddr_in &address) {
        if (::bind(fd_, reinterpret_cast<sockaddr *>(&address), sizeof(address)) < 0) {
            throw std::system_error(errno, std::system_category(), "bind failed");
        }
    }

    void listen(int backlog) {
        if (::listen(fd_, backlog) < 0) {
            throw std::system_error(errno, std::system_category(), "listen failed");
        }
    }

    int accept(sockaddr_in &client_addr) {
        socklen_t addr_len = sizeof(client_addr);
        int client_fd = ::accept(fd_, reinterpret_cast<sockaddr *>(&client_addr), &addr_len);
        if (client_fd < 0) {
            throw std::system_error(errno, std::system_category(), "accept failed");
        }
        return client_fd;
    }

    operator int() const {
        return fd_;
    }
};

class ClientConnection {
    int fd_;

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

        ssize_t n = recv(fd_, buffer.data() + total_read, size - total_read, 0);
        if (n <= 0) {
            throw std::runtime_error("read error or connection closed");
        }
        buffer.resize(static_cast<size_t>(n));
        return buffer;
    }

    void write_full(const char *data, size_t size) {
        size_t total_sent = 0;
        while (total_sent < size) {
            ssize_t n = send(fd_, data + total_sent, size - total_sent, 0);
            if (n <= 0) {
                throw std::runtime_error("write error");
            }
            total_sent += n;
        }
    }

    operator int() const {
        return fd_;
    }
};

int main(int argc, char *argv[]) {
    // Disable output buffering
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
    try {
        Socket server_fd(AF_INET, SOCK_STREAM, 0);
        // Since the tester restarts your program quite often, setting SO_REUSEADDR
        // ensures that we don't run into 'Address already in use' errors
        int reuse = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
            close(server_fd);
            std::cerr << "setsockopt failed: " << std::endl;
            return 1;
        }
        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(9092);

        server_fd.bind(server_addr);
        server_fd.listen(5);
        std::cout << "Listening on port 9092...\n";
        sockaddr_in client_addr{};

        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = server_fd.accept(client_addr);
        ClientConnection client(client_fd);
        try {
            std::cout << "Client connected\n";
            auto buffer = client.read_full();

            ResponseHandler response_handler(buffer.data());
            const char *response_buffer = response_handler.GetResponseBuffer();
            size_t response_size = response_handler.GetResponseSize();
            client.write_full(response_buffer, response_size);
        } catch (const std::exception &e) {
            std::cerr << "Error handling client: " << e.what() << "\n";
        }
    } catch (const std::exception &e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }

    return 0;
}

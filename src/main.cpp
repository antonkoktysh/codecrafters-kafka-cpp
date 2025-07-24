#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

#include "Socket.h"

int main(int argc, char* argv[]) {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    try {
        Socket server_fd(AF_INET, SOCK_STREAM, 0);
        // Since the tester restarts your program quite often, setting SO_REUSEADDR
        // ensures that we don't run into 'Address already in use' errors
        int reuse = 1;
        if (setsockopt(static_cast<int>(server_fd), SOL_SOCKET, SO_REUSEADDR, &reuse,
                       sizeof(reuse)) < 0) {
            // TODO exception
            throw ServerException("setsockopt failed");
        }
        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(9092);

        server_fd.bind(server_addr);
        int connection_backlog = 5;
        server_fd.listen(connection_backlog);

        std::cout << "Listening on port 9092..." << std::endl;

        std::vector<std::thread> workers;
        while (true) {
            try {
                struct sockaddr_in client_addr{};

                socklen_t client_addr_len = sizeof(client_addr);
                int client_fd = server_fd.accept(client_addr);
                std::unique_ptr<ClientConnection> client =
                    std::make_unique<ClientConnection>(client_fd);
                workers.emplace_back(HandleClient, std::move(client));
            } catch (std::exception& e) {
                std::cout << "Client Error occured" << e.what() << std::endl;
                break;
            }
        }

        for (auto& t : workers) {
            if (t.joinable())
                t.join();
        }

    } catch (const std::exception& e) {
        std::cout << "Server error: " << e.what() << std::endl;
    }
    return 0;
}

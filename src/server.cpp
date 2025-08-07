#include "server.h"
Server::Server(Socket& socket) : server_socket_(socket) {
    sockaddr_in server_addr{};
    server_addr.sin_family = ServerConstants::kDefaultAddressFamily;
    server_addr.sin_addr.s_addr = ServerConstants::kDefaultBindAddress;
    server_addr.sin_port = htons(ServerConstants::kDefaultPort);

    int reuse = 1;
    if (setsockopt(static_cast<int>(server_socket_), SOL_SOCKET, SO_REUSEADDR, &reuse,
                   sizeof(reuse)) < 0) {
        throw ServerException("setsockopt failed");
    }

    server_socket_.Bind(server_addr);
}

void Server::Start() {
    int connection_backlog = 5;
    server_socket_.Listen(connection_backlog);

    std::cout << "Listening on port 9092..." << std::endl;

    while (true) {
        try {
            struct sockaddr_in client_addr{};
            int client_fd = server_socket_.Accept(client_addr);
            worker_threads_.emplace_back(HandleClient,
                                         std::make_unique<ClientConnection>(client_fd));
        } catch (std::exception& e) {
            std::cout << "Client Error occurred: " << e.what() << std::endl;
        }
    }

    for (auto& t : worker_threads_) {
        if (t.joinable()) {
            t.join();
        }
    }
}

#include "client_connection.h"

void HandleClient(std::unique_ptr<ClientConnection> client) {
    while (client->IsActive()) {
        try {
            auto buffer = client->read_full();
            auto factory = ReaderFactoryCreator::CreateFactory(buffer);
            auto reader = factory->Create(buffer);
            auto request = reader->ParseRequest();
            auto response = request->Process();
            auto to_send = response->ByteResponse();
            client->write_full(to_send);
        } catch (ClientDisconnectedError& e) {
            // OnClientDisconnected();
            std::cout << e.what() << std::endl;
            break;
        }
    }
}

bool ClientConnection::IsActive() const {
    return is_active_;
}
void ClientConnection::write_full(std::vector<std::byte>& data) {
    write(data, data.size());
}
void ClientConnection::write(std::vector<std::byte>& data, size_t size) {
    if (::write(fd_, data.data(), size) < 0) {
        throw std::runtime_error("Error occured during writing");
    }
}

std::vector<std::byte> ClientConnection::read_full() {
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

ClientConnection::~ClientConnection() {
    if (fd_ >= 0) {
        close(fd_);
    }
}

ClientConnection::ClientConnection(int fd) : fd_(fd) {
}

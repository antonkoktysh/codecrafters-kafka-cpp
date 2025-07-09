#pragma once
#include <stdexcept>
#include <string>
class ServerException : public std::runtime_error {
public:
    explicit ServerException(const std::string& what) : std::runtime_error(what) {
    }
};

class SocketCreationError : public ServerException {
public:
    explicit SocketCreationError() : ServerException("Socket creation failed: ") {
    }
};

class BindError : public ServerException {
public:
    explicit BindError() : ServerException("Bind failed ") {
    }
};

class ListenError : public ServerException {
public:
    explicit ListenError() : ServerException("Listen failed ") {
    }
};

class AcceptError : public ServerException {
public:
    explicit AcceptError() : ServerException("Accept failed: ") {
    }
};

class ClientException : public std::runtime_error {
public:
    explicit ClientException(const std::string& what) : std::runtime_error(what) {
    }
};
class ClientDisconnectedError : public ClientException {
public:
    ClientDisconnectedError() : ClientException("Client closed connection") {
    }
};

class ClientReceiveError : public ClientException {
public:
    explicit ClientReceiveError() : ClientException("Receive error ") {
    }
};

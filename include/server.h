#pragma once
#include "Socket.h"
#include "client_connection.h"
#include <thread>
#include <vector>
#include <iostream>
#include "server_constants.h"
class Server {
public:
    Server(Socket& socket);
    void Start();

private:
    Socket server_socket_;
    std::vector<std::thread> worker_threads_;
};


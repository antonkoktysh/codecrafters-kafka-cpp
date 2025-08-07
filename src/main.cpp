#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>

#include "Socket.h"
#include "server.h"

int main(int argc, char* argv[]) {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    try {
        Socket server_fd(ServerConstants::kDefaultAddressFamily, SOCK_STREAM, 0);
        Server server(server_fd);
        server.Start();
    } catch (const std::exception& e) {
        std::cout << "Server error: " << e.what() << std::endl;
    }
    return 0;
}

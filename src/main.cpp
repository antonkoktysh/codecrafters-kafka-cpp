#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
const int16_t MIN_API_VERSIONS = 0;
const int16_t MAX_API_VERSIONS = 4;
const int16_t UNSUPPORTED_VERSION_ERROR = 35;
int main(int argc, char *argv[]) {
  // Disable output buffering
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    std::cerr << "Failed to create server socket: " << std::endl;
    return 1;
  }

  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
      0) {
    close(server_fd);
    std::cerr << "setsockopt failed: " << std::endl;
    return 1;
  }

  struct sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(9092);

  if (bind(server_fd, reinterpret_cast<struct sockaddr *>(&server_addr),
           sizeof(server_addr)) != 0) {
    close(server_fd);
    std::cerr << "Failed to bind to port 9092" << std::endl;
    return 1;
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    close(server_fd);
    std::cerr << "listen failed" << std::endl;
    return 1;
  }

  std::cout << "Waiting for a client to connect...\n";

  struct sockaddr_in client_addr{};
  socklen_t client_addr_len = sizeof(client_addr);

  // You can use print statements as follows for debugging, they'll be visible
  // when running tests.
  std::cerr << "Logs from your program will appear here!\n";

  // Uncomment this block to pass the first stage
  int client_fd =
      accept(server_fd, reinterpret_cast<struct sockaddr *>(&client_addr),
             &client_addr_len);
  std::cout << "Client connected\n";

  char buffer[1024];
  ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
  if (bytes_received < 0) {
    std::cout << "Failed to receive data from client\n";
    close(client_fd);
    close(server_fd);
    return 1;
  }

  if (bytes_received < 12) {
    std::cerr << "Expected at least 12 bytes" << std::endl;
    close(client_fd);
    close(server_fd);
    return 1;
  }

  char response[10];

  int16_t request_api_key;
  int16_t request_api_version;
  uint32_t correlation_id;
  memcpy(&request_api_key, buffer + 4, sizeof(request_api_key));
  memcpy(&request_api_version, buffer + 6, sizeof(request_api_version));
  memcpy(&correlation_id, buffer + 8, sizeof(correlation_id));

  request_api_key = ntohs(request_api_key);
  request_api_version = ntohs(request_api_version);

  std::cerr << "Received request - API Key: " << request_api_key
            << ", API Version: " << request_api_version
            << ", Correlation ID: " << htonl(correlation_id) << std::endl;

  // std::cout << correlation_id << std::endl;
  // std::cout << htonl(correlation_id) << std::endl;
  // std::cout << htonl(htonl(correlation_id)) << std::endl;

  // 1. message_size: 4 bytes, any value
  uint32_t message_size = 54;
  uint32_t net_message_size = htonl(message_size);  // To big-endian
  memcpy(response, &net_message_size, sizeof(net_message_size));

  int16_t error_code = 0;
  if (request_api_key == 18 && (request_api_version < MIN_API_VERSIONS ||
                                request_api_version > MAX_API_VERSIONS)) {
    error_code = UNSUPPORTED_VERSION_ERROR;
  }

  int16_t net_error_code = htons(error_code);
  memcpy(response, &net_message_size, sizeof(net_message_size));
  memcpy(response + 4, &correlation_id, sizeof(correlation_id));
  memcpy(response + 8, &net_error_code, sizeof(net_error_code));

  ssize_t bytes_sent = send(client_fd, response, sizeof(response), 0);
  if (bytes_sent != sizeof(response)) {
    std::cerr << "Failed to send full response\n";
  }

  close(client_fd);
  close(server_fd);
  return 0;
}

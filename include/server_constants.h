#pragma once

#include <netinet/in.h>

namespace ServerConstants {
constexpr in_port_t kDefaultPort = 9092;
constexpr int kDefaultBacklog = 5;
constexpr sa_family_t kDefaultAddressFamily = AF_INET;
constexpr in_addr_t kDefaultBindAddress = INADDR_ANY;

enum class Protocol : int { TCP = SOCK_STREAM, UDP = SOCK_DGRAM };
}  // namespace ServerConstants

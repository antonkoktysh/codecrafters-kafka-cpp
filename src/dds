#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

class ResponseBuilder {
 private:
  size_t header_size_{};
  size_t body_size_{};
  size_t response_size_{};
  char response_buffer_[1024]{};
  void SetResponseHeaderSize(size_t _header_size) {
    header_size_ = _header_size;
  }
  void SetResponseBodySize(size_t _body_size) { body_size_ = _body_size; }
  void SetResponseSize(size_t _response_size) {
    response_size_ = _response_size;
  }
  [[nodiscard]] size_t GetResponseHeaderSize() const { return header_size_; }
  [[nodiscard]] size_t GetResponseBodySize() const { return body_size_; }

  void BuildResponseHeader(const char *buffer) {
    memcpy(response_buffer_ + 4, buffer + 8,
           sizeof(int32_t));  // Correlation id
    SetResponseHeaderSize(sizeof(int32_t));
  }
  void BuildResponseBody(const char *buffer) {
    size_t header_buf = 8;
    size_t body_size = header_buf;
    int16_t request_api_key;
    int16_t request_api_version;
    memcpy(&request_api_key, (buffer + 4),
           sizeof(request_api_key));  // Request API key
    if (ntohs(request_api_key) != 18) {
      std::cerr << "Wrong API key request: " << ntohs(request_api_key) << '\n';
      return;
    }
    memcpy(&request_api_version, buffer + 6,
           sizeof(request_api_version));  // Request API version
    int16_t error_code = request_api_version == ntohs(4) ? htons(0) : htons(35);
    int32_t throttle_time_ms = htonl(0);
    int16_t min_version = htons(1);
    int16_t max_version = htons(4);
    int8_t num_api_keys = 1;
    int8_t tag_buffer = 0;
    if (ntohs(error_code) == 0) {
      num_api_keys = 2;
    }
    memcpy(response_buffer_ + body_size, &error_code, sizeof(error_code));
    body_size += sizeof(error_code);
    memcpy(response_buffer_ + body_size, &num_api_keys, sizeof(num_api_keys));
    body_size += sizeof(num_api_keys);
    if (num_api_keys > 1) {
      memcpy(response_buffer_ + body_size, &request_api_key,
             sizeof(request_api_key));
      body_size += sizeof(request_api_key);
      memcpy(response_buffer_ + body_size, &min_version, sizeof(min_version));
      body_size += sizeof(min_version);
      memcpy(response_buffer_ + body_size, &max_version, sizeof(max_version));
      body_size += sizeof(max_version);
      memcpy(response_buffer_ + body_size, &tag_buffer, sizeof(tag_buffer));
      body_size += sizeof(tag_buffer);
    }
    memcpy(response_buffer_ + body_size, &throttle_time_ms,
           sizeof(throttle_time_ms));
    body_size += sizeof(throttle_time_ms);
    memcpy(response_buffer_ + body_size, &tag_buffer, sizeof(tag_buffer));
    body_size += sizeof(tag_buffer);
    SetResponseBodySize(body_size - header_buf);
  }

 public:
  const char *BuildResponse(const char *buffer) {
    BuildResponseHeader(buffer);
    BuildResponseBody(buffer);
    int32_t message_size = GetResponseHeaderSize() + GetResponseBodySize();
    message_size = htonl(message_size);
    size_t response_size =
        sizeof(message_size) + GetResponseHeaderSize() + GetResponseBodySize();
    memcpy(response_buffer_, &message_size, sizeof(message_size));
    SetResponseSize(response_size);
    return response_buffer_;
  }
  [[nodiscard]] size_t GetResponseSize() const { return response_size_; }
};

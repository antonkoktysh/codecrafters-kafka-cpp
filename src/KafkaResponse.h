#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>
enum class KafkaErrorCode : int16_t {
    NONE = 0,
    UNSUPPORTED_VERSION = 35,
    INVALID_REQUEST = 42,
    UNKNOWN_SERVER_ERROR = 48,
};
bool operator==(KafkaErrorCode a, int16_t b) {
    return static_cast<int16_t>(a) == b;
}

bool operator==(int16_t a, KafkaErrorCode b) {
    return a == static_cast<int16_t>(b);
}

enum class KafkaApiKey : int16_t {
    PRODUCE = 0,
    FETCH = 1,
    LIST_OFFSETS = 2,
    METADATA = 3,
    LEADER_AND_ISR = 4,
    STOP_REPLICA = 5,
    API_VERSIONS = 18,
};

bool operator==(KafkaApiKey a, int16_t b) {
    return static_cast<int16_t>(a) == b;
}

bool operator==(int16_t a, KafkaApiKey b) {
    return a == static_cast<int16_t>(b);
}
bool operator!=(KafkaApiKey a, int16_t b) {
    return static_cast<int16_t>(a) != b;
}

bool operator!=(int16_t a, KafkaApiKey b) {
    return a != static_cast<int16_t>(b);
}

class ResponseHandler {
private:
    size_t header_size_;
    size_t body_size_;
    size_t response_size_;
    char response_buffer_[1024];
    const char *buffer_;

    template <typename T>
    void Append(size_t offset, T &value) {
        memcpy(response_buffer_ + offset, &value, sizeof(value));
    }
    template <typename T>
    void Append(size_t offset, T *value, size_t count) {
        memcpy(response_buffer_ + offset, value, count);
    }
    void BuildResponseBody(const char *buffer) {
        int16_t request_api_key;
        int16_t request_api_version;
        memcpy(&request_api_key, (buffer + 4),
               sizeof(request_api_key));  // Request API key
        if (ntohs(request_api_key) != KafkaApiKey::API_VERSIONS) {
            std::cerr << "Wrong API key request: " << ntohs(request_api_key) << '\n';
            throw std::runtime_error("Wrong API key request: " +
                                     std::to_string(htons(request_api_key)));
            // TODO
            // Close fd
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
        size_t offset = 8;
        Append(offset, error_code);
        offset += sizeof(error_code);
        Append(offset, num_api_keys);
        offset += sizeof(num_api_keys);

        if (num_api_keys > 1) {
            Append(offset, request_api_key);
            offset += sizeof(request_api_key);

            Append(offset, min_version);
            offset += sizeof(min_version);

            Append(offset, max_version);
            offset += sizeof(max_version);

            Append(offset, tag_buffer);
            offset += sizeof(tag_buffer);
        }

        Append(offset, throttle_time_ms);
        offset += sizeof(throttle_time_ms);

        Append(offset, tag_buffer);
        offset += sizeof(tag_buffer);

        body_size_ = offset - 2 * sizeof(int32_t);
    }
    void BuildResponse() {
        Append(4, buffer_ + 8, sizeof(int32_t));
        header_size_ = sizeof(int32_t);

        BuildResponseBody(buffer_);
        int32_t message_size = header_size_ + body_size_;
        message_size = htonl(message_size);
        response_size_ = sizeof(message_size) + header_size_ + body_size_;
        memcpy(response_buffer_, &message_size, sizeof(message_size));
    }

public:
    const char *GetResponseBuffer() {
        return response_buffer_;
    }

    ResponseHandler(char *buffer) : buffer_(buffer) {
        BuildResponse();
    }
    [[nodiscard]] size_t GetResponseSize() const {
        return response_size_;
    }
};

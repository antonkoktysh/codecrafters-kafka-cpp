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

enum class KafkaApiKey : int16_t {
    PRODUCE = 0,
    FETCH = 1,
    LIST_OFFSETS = 2,
    METADATA = 3,
    LEADER_AND_ISR = 4,
    STOP_REPLICA = 5,
    API_VERSIONS = 18,
};

struct ApiVersion {
    int16_t api_key = 75;
    int16_t min_supported_version = htons(0);
    int16_t max_supported_version = htons(0);
    int8_t tag_buffer = 0;
};

class IRequestHandler {
public:
    virtual size_t ResponseSize() const {
    }
    virtual std::vector<char> GetResponseBuffer() const {
    }
    virtual ~IRequestHandler() = default;
};

class ApiVersionsHandler : public IRequestHandler {
private:
    size_t header_size_;
    size_t body_size_;
    size_t response_size_;
    std::vector<char> response_buffer_;
    std::vector<char> buffer_;
    template <typename T>
    void Append(size_t offset, T& value, size_t count) {
        memcpy(response_buffer_.data() + offset, &value, count);
    }

    void HandleRequest() {
        Append(4, buffer_[8], sizeof(int32_t));
        header_size_ = sizeof(int32_t);

        BuildResponseBody();
        int32_t message_size = header_size_ + body_size_;
        message_size = htonl(message_size);
        response_size_ = sizeof(message_size) + header_size_ + body_size_;
        memcpy(response_buffer_.data(), &message_size, sizeof(message_size));
        response_buffer_.resize(response_buffer_.size());
    }

    void BuildResponseBody() {
        int16_t request_api_key;
        int16_t request_api_version;
        memcpy(&request_api_key, (buffer_.data() + 4),
               sizeof(request_api_key));  // Request API key
        if (ntohs(request_api_key) != static_cast<int>(KafkaApiKey::API_VERSIONS)) {
            std::cerr << "Wrong API key request: " << ntohs(request_api_key) << '\n';
            throw std::runtime_error("Wrong API key request: " +
                                     std::to_string(htons(request_api_key)));
        }

        memcpy(&request_api_version, buffer_.data() + 6,
               sizeof(request_api_version));  // Request API version
        int16_t error_code = request_api_version == ntohs(4) ? htons(0) : htons(35);
        int32_t throttle_time_ms = htonl(0);
        int8_t num_api_keys = 3;
        int8_t tag_buffer = 0;
        size_t offset = 8;

        Append(offset, error_code, sizeof(error_code));
        offset += sizeof(error_code);
        Append(offset, num_api_keys, sizeof(num_api_keys));
        offset += sizeof(num_api_keys);

        std::vector<ApiVersion> api_versions;
        api_versions.emplace_back(htons(18), htons(1), htons(4), 0);
        api_versions.emplace_back(htons(75), htons(0), htons(0), 0);

        for (size_t ind = 0; ind < api_versions.size(); ++ind) {

            Append(offset, api_versions[ind].api_key, sizeof(api_versions[ind].api_key));
            offset += sizeof(api_versions[ind].api_key);

            Append(offset, api_versions[ind].min_supported_version,
                   sizeof(api_versions[ind].api_key));
            offset += sizeof(api_versions[ind].min_supported_version);

            Append(offset, api_versions[ind].max_supported_version,
                   sizeof(api_versions[ind].api_key));
            offset += sizeof(api_versions[ind].max_supported_version);

            Append(offset, api_versions[ind].tag_buffer, sizeof(api_versions[ind].tag_buffer));
            offset += sizeof(api_versions[ind].tag_buffer);
        }

        Append(offset, throttle_time_ms, sizeof(throttle_time_ms));
        offset += sizeof(throttle_time_ms);

        Append(offset, tag_buffer, sizeof(tag_buffer));
        offset += sizeof(tag_buffer);

        body_size_ = offset - 2 * sizeof(int32_t);
    }

public:
    ApiVersionsHandler(const std::vector<char>& buffer) : buffer_(buffer) {
        response_buffer_.resize(1024);

        HandleRequest();
    }
    size_t ResponseSize() const override {
        return response_size_;
    }
    std::vector<char> GetResponseBuffer() const override {
        return response_buffer_;
    }
    virtual ~ApiVersionsHandler() = default;
};

struct Topic {
    int8_t topic_name_len;
    char* topic_name;
    int8_t tag_buffer = 0;
};

class DescribeTopicPartitions : public IRequestHandler {
public:
    DescribeTopicPartitions(const std::vector<char>& buffer) : buffer_(buffer) {
        response_buffer_.resize(1024);
    }

    size_t ResponseSize() const override {
        return response_size_;
    }

    std::vector<char> GetResponseBuffer() const override {
        return response_buffer_;
    }
    ~DescribeTopicPartitions() = default;

private:
    size_t header_size_ = 8;
    size_t body_size_;
    size_t response_size_;
    std::vector<char> response_buffer_;
    std::vector<char> buffer_;

    size_t request_header_size_ 0;
    template <typename T>
    void Append(size_t offset, T& value, size_t count) {
        memcpy(response_buffer_.data() + offset, &value, count);
    }
    void HandleRequest() {
        int32_t message_size = header_size_ + body_size_;
        response_size_ = message_size + sizeof(int32_t);
        memcpy(response_buffer_.data(), &message_size, sizeof(message_size));  // messege_size

        BuildResponseHeader();
    }

    void BuildResponseHeader() {
        int16_t api_key;
        memcpy(&api_key, buffer_.data() + 4, sizeof(int16_t));

        int16_t api_version;
        memcpy(&api_key, buffer_.data() + 6, sizeof(int16_t));

        int32_t correlation_id;
        memcpy(&correlation_id, buffer_.data() + 8, sizeof(int32_t));

        int16_t client_id_len;
        memcpy(&client_id_len, buffer_.data() + 12, sizeof(int16_t));

        std::string contents(client_id_len - 1, 's');
        memcpy(contents.data(), buffer_.data() + 14, client_id_len - 1);

        int8_t tag_buffer = 0;

        Append(4, correlation_id, sizeof(int32_t));
        Append(8, tag_buffer, sizeof(int8_t));
        request_header_size_ = 10 + contents.size();
    }
    void BuildResponseBody() {
        int8_t topics_array_len;
    }
};

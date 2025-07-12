#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

struct ApiVersion {
    int16_t api_key = 75;
    int16_t min_supported_version = htons(0);
    int16_t max_supported_version = htons(0);
    int8_t tag_buffer = 0;
};

class IRequestHandler {
public:
    virtual size_t ResponseSize() const = 0;
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
        if (ntohs(request_api_key) != 18) {
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

class DescribeTopicPartitionsHandler : public IRequestHandler {
public:
    DescribeTopicPartitionsHandler(const std::vector<char>& buffer) : buffer_(buffer) {
        response_buffer_.resize(1024);
        HandleRequest();
    }

    size_t ResponseSize() const override {
        return response_size_;
    }

    std::vector<char> GetResponseBuffer() const override {
        return response_buffer_;
    }
    ~DescribeTopicPartitionsHandler() = default;

private:
    size_t header_size_ = 8;
    size_t body_size_;
    size_t response_size_;
    std::vector<char> response_buffer_;
    std::vector<char> buffer_;

    size_t request_header_size_ = 0;
    template <typename T>
    void Append(size_t offset, T& value, size_t count) {
        memcpy(response_buffer_.data() + offset, &value, count);
    }
    void HandleRequest() {
        BuildResponseHeader();
        BuildResponseBody();
        int32_t message_size = response_size_ - 4;
        message_size = htonl(message_size);
        memcpy(response_buffer_.data(), &message_size, sizeof(message_size));  // message_size
    }

    void BuildResponseHeader() {
        request_header_size_ = 0;
        int16_t api_key;
        memcpy(&api_key, buffer_.data() + 4, sizeof(int16_t));
        request_header_size_ += sizeof(int16_t);

        int16_t api_version;
        memcpy(&api_key, buffer_.data() + 6, sizeof(int16_t));
        request_header_size_ += sizeof(int16_t);

        int32_t correlation_id;
        memcpy(&correlation_id, buffer_.data() + 8, sizeof(int32_t));
        request_header_size_ += sizeof(int32_t);

        int16_t client_id_len;
        memcpy(&client_id_len, buffer_.data() + 12, sizeof(int16_t));
        request_header_size_ += sizeof(int16_t);

        std::vector<char> contents(ntohs(client_id_len));
        memcpy(contents.data(), buffer_.data() + 14, ntohs(client_id_len));
        request_header_size_ += contents.size();

        int8_t tag_buffer = 0;
        request_header_size_ += sizeof(tag_buffer);

        Append(4, correlation_id, sizeof(int32_t));
        Append(8, tag_buffer, sizeof(int8_t));
    }

    void BuildResponseBody() {
        size_t request_offset = request_header_size_ + sizeof(int32_t);

        int8_t topics_array_len;
        memcpy(&topics_array_len, buffer_.data() + request_offset, sizeof(topics_array_len));
        request_offset += sizeof(topics_array_len);

        int8_t topic_name_len;
        memcpy(&topic_name_len, buffer_.data() + request_offset, sizeof(topic_name_len));
        request_offset += sizeof(topic_name_len);

        std::vector<char> topic_name(topic_name_len - 1);
        memcpy(topic_name.data(), buffer_.data() + request_offset, topic_name_len - 1);
        request_offset += topic_name.size();

        int8_t topic_tag_buffer;
        memcpy(&topic_tag_buffer, buffer_.data() + request_offset, sizeof(topic_tag_buffer));
        request_offset += sizeof(topic_tag_buffer);

        request_offset += sizeof(int32_t);
        int8_t next_cursor;
        memcpy(&next_cursor, buffer_.data() + request_offset, sizeof(next_cursor));
        request_offset += 1;

        size_t response_offset = 9;
        int32_t throttle_time_ms = htonl(0);
        Append(response_offset, throttle_time_ms, sizeof(throttle_time_ms));
        response_offset += sizeof(throttle_time_ms);

        Append(response_offset, topics_array_len, sizeof(topics_array_len));
        response_offset += sizeof(topics_array_len);

        int16_t error_code = htons(3);
        Append(response_offset, error_code, sizeof(error_code));
        response_offset += sizeof(error_code);

        memcpy(response_buffer_.data() + response_offset, &topic_name_len, sizeof(int8_t));
        response_offset += sizeof(int8_t);
        memcpy(response_buffer_.data() + response_offset, topic_name.data(), topic_name.size());
        response_offset += topic_name.size();

        // Topic ID
        std::array<char, 16> topic_id;
        topic_id.fill(0);
        memcpy(response_buffer_.data() + response_offset, topic_id.data(), topic_id.size());
        response_offset += 16;

        int8_t is_internal = 0;
        Append(response_offset, is_internal, sizeof(is_internal));
        response_offset += sizeof(is_internal);

        int8_t patrtitions_array = 1;
        Append(response_offset, patrtitions_array, sizeof(patrtitions_array));
        response_offset += sizeof(patrtitions_array);

        int32_t operations = 0;
        Append(response_offset, operations, sizeof(operations));
        response_offset += sizeof(operations);

        int8_t tag_buffer = 0;
        Append(response_offset, tag_buffer, sizeof(tag_buffer));
        response_offset += sizeof(tag_buffer);

        Append(response_offset, next_cursor, sizeof(next_cursor));
        response_offset += sizeof(next_cursor);

        Append(response_offset, tag_buffer, sizeof(tag_buffer));
        response_offset += sizeof(tag_buffer);
        response_size_ = response_offset;
    }
};

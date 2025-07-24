#pragma once
#include <cstdint>
#include <string>

#include "response.h"
struct RequestHeader {
    int16_t api_key = 0;
    int16_t api_version = 0;
    int32_t correlation_id = 0;
    int16_t client_id_len = 0;
    std::string client_id_contents;
    int8_t tag_buffer = 0;
};

// Abstract class for request
class Request {
public:
    Request(int32_t message_size, const RequestHeader& header)
        : message_size_(message_size), header_(header) {
    }
    virtual std::unique_ptr<Response> Process() = 0;

protected:
    int32_t message_size_ = 0;
    RequestHeader header_;
};

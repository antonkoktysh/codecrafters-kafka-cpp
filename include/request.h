#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include "response.h"
struct RequestHeader {
    int16_t api_key;
    int16_t api_version;
    int32_t correlation_id;
    std::string client_id_contents;
    int8_t tag_buffer;
};

// Abstract class for request
class Request {
public:
    Request(int32_t message_size, const RequestHeader& header)
        : message_size_(message_size), header_(header) {
    }
    virtual std::unique_ptr<Response> Process() = 0;
    virtual ~Request() = default;

protected:
    int32_t message_size_;
    RequestHeader header_;
};

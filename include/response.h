#pragma once
#include <vector>
#include <cstddef>
#include <cstdint>
#include <optional>

struct ResponseHeader {
    int32_t correlation_id;
    std::optional<int8_t> tag_buffer = std::nullopt;  // for v1
    ResponseHeader() = default;
    ResponseHeader(int32_t corr_id) : correlation_id(corr_id) {
    }
    ResponseHeader(int32_t corr_id, int8_t tag_buffer)
        : correlation_id(corr_id), tag_buffer(tag_buffer) {
    }
};

// Abstract class for response
class Response {
public:
    virtual ~Response() = default;
    Response(int32_t message_size, const ResponseHeader& header)
        : message_size_(message_size), header_(header) {
    }
    virtual std::vector<std::byte> ByteResponse() = 0;

protected:
    int32_t message_size_;
    ResponseHeader header_;
};

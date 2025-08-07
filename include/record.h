#pragma once

#include <cstdint>
#include <vector>
#include <optional>

struct Header {};

struct Record {
    std::int8_t length_;
    std::int8_t attributes_;
    std::int8_t timestamp_delta_;
    std::int8_t offset_delta_;
    // key
    std::vector<std::byte> value;
    // int8_t header_array_count;
    //std::vector<Header> headers;
};

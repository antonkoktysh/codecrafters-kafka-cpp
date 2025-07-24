#pragma once
#include "request.h"
#include "api_versions_request.h"
#include <vector>
#include <cstddef>
#include <stdexcept>
class Reader {
private:
    std::vector<std::byte> data_;
    size_t offset_ = 0;

public:
    Reader(const std::vector<std::byte>& data, size_t offset = 0) : data_(data), offset_(offset) {
    }

    template <typename IntType>
    IntType ReadInt() {
        IntType value;
        std::memcpy(&value, data_.data() + offset_, sizeof(IntType));
        offset_ += sizeof(IntType);
        return value;
    }

    int8_t ReadInt8() {
        return ReadInt<int8_t>();
    }

    int16_t ReadInt16() {
        return ReadInt<int16_t>();
    }

    int32_t ReadInt32() {
        return ReadInt<int32_t>();
    }
    int64_t ReadInt64() {
        return ReadInt<int64_t>();
    }

    __int128_t ReadInt128() {
        return ReadInt<__int128_t>();
    }
    std::string ReadString(size_t count) {
        std::string str(count, ' ');
        std::memcpy(str.data(), data_.data() + offset_, count);
        offset_ += count;
        return str;
    }
};

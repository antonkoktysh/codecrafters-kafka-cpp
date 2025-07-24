#pragma once
#include <vector>
#include <cstddef>
#include <concepts>
#include <type_traits>

class Writer {
public:
    Writer(std::vector<std::byte>& data, size_t offset = 0) : data_(data), offset_(offset) {
    }

    template <std::integral IntType>
    void WriteInt(IntType value) {
        std::memcpy(data_.data() + offset_, &value, sizeof(value));
        offset_ += sizeof(value);
    }

    void WriteInt8(int8_t value) {
        WriteInt<int8_t>(value);
    }

    void WriteInt16(int16_t value) {
        WriteInt<int16_t>(value);
    }
    void WriteInt32(int32_t value) {
        WriteInt<int32_t>(value);
    }
    void WriteInt64(int64_t value) {
        WriteInt<int64_t>(value);
    }
    void WriteInt128(__int128_t value) {
        WriteInt<__int128_t>(value);
    }
    void WriteString(const std::string& str) {
        std::memcpy(data_.data() + offset_, str.data(), str.size());
        offset_ += str.size();
    }

private:
    std::vector<std::byte>& data_;
    size_t offset_ = 0;
};

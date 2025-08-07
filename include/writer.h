#pragma once
#include <vector>
#include <cstddef>
#include <cstring>
#include <concepts>
#include <string>


class Writer {
public:
    Writer(std::vector<std::byte>& data, size_t offset = 0);
    std::size_t TotalWritten() const;
    template <std::integral IntType>
    void WriteInt(IntType value);

    void WriteInt8(int8_t value);
    void WriteInt16(int16_t value);

    void WriteInt32(int32_t value);
    void WriteInt64(int64_t value);

    void WriteInt128(__int128_t value);

    void WriteString(const std::string& str);
    void WriteMessageSize(int32_t message_size);

private:
    void EnsureCapacity(size_t required_size);

    std::vector<std::byte>& data_;
    std::size_t offset_;
    std::size_t total_;
};

template <std::integral IntType>
void Writer::WriteInt(IntType value) {
    EnsureCapacity(sizeof(value));
    std::memcpy(data_.data() + offset_, &value, sizeof(value));
    offset_ += sizeof(value);
    total_ += sizeof(value);
}
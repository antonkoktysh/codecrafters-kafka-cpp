#pragma once
#include <vector>
#include <cstddef>
#include <cstdint>
#include <string>
#include <cstring>

class Reader {
private:
    std::vector<std::byte> data_;
    std::size_t offset_;

public:
    Reader(const std::vector<std::byte>& data, std::size_t offset = 0);
    Reader(const std::string&, std::size_t offset = 0);
    template <typename T>
    void Read(T* dst, std::size_t size) {
        std::memcpy(dst, data_.data() + offset_, size);
        offset_ += size;
    }

    template <typename IntType>
    IntType ReadInt();

    std::uint32_t ReadUnsignedInt();
    std::int32_t ReadVarint();

    int8_t ReadInt8();

    int16_t ReadInt16();

    int32_t ReadInt32();
    int64_t ReadInt64();

    __int128_t ReadInt128();
    std::string ReadString(std::size_t count);

    size_t Remaining() const {
        return data_.size() - offset_;
    }
};


template <typename IntType>
IntType Reader::ReadInt() {
    IntType value;
    std::memcpy(&value, data_.data() + offset_, sizeof(IntType));
    offset_ += sizeof(IntType);
    return value;
}

#include "reader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
Reader::Reader(const std::vector<std::byte>& data, std::size_t offset)
    : data_(data), offset_(offset) {
}

Reader::Reader(const std::string& file_path, std::size_t offset) : offset_(offset) {
    std::ifstream file(file_path);
    std::stringstream ss;
    ss << file.rdbuf();
    const std::string& str = ss.str();
    data_.resize(str.size());
    std::memcpy(data_.data(), str.data(), str.size());
    std::cout << std::hex;
    for (std::byte b : data_) {
        std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(b) << " ";
    }
    std::cout << std::dec << "\n";
}

int8_t Reader::ReadInt8() {
    return ReadInt<int8_t>();
}

int16_t Reader::ReadInt16() {
    return ReadInt<int16_t>();
}
int32_t Reader::ReadInt32() {
    return ReadInt<int32_t>();
}

int64_t Reader::ReadInt64() {
    return ReadInt<int64_t>();
}

__int128_t Reader::ReadInt128() {
    return ReadInt<__int128_t>();
}
std::string Reader::ReadString(std::size_t count) {
    std::string str(count, ' ');
    std::memcpy(str.data(), data_.data() + offset_, count);
    offset_ += count;
    return str;
}

std::uint32_t Reader::ReadUnsignedInt() {
    std::uint32_t n = 0;
    for (std::size_t i = 0;; i += 7) {
        unsigned char c;
        Read(&c, sizeof(c));
        n += (c & 0x7F) << i;
        if (!(c & 0x80)) {
            return n;
        }
    }
}

std::int32_t Reader::ReadVarint() {
    std::uint32_t n = ReadUnsignedInt();
    return (n & 1) ? -((n >> 1) + 1) : (n >> 1);
}

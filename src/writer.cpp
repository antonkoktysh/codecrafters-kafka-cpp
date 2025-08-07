#include "writer.h"

void Writer::EnsureCapacity(size_t required_size) {
    if (offset_ + required_size > data_.size()) {
        data_.resize(offset_ + required_size);
    }
}

std::size_t Writer::TotalWritten() const {
    return total_;
}

void Writer::WriteMessageSize(int32_t message_size) {
    EnsureCapacity(sizeof(message_size));
    std::memcpy(data_.data(), &message_size, sizeof(message_size));
    total_ += sizeof(message_size);
}

Writer::Writer(std::vector<std::byte>& data, size_t offset)
    : data_(data), offset_(offset), total_(0) {
}


void Writer::WriteInt8(int8_t value) {
    WriteInt<int8_t>(value);
}
void Writer::WriteInt16(int16_t value) {
    WriteInt<int16_t>(value);
}

void Writer::WriteInt32(int32_t value) {
    WriteInt<int32_t>(value);
}
void Writer::WriteInt64(int64_t value) {
    WriteInt<int64_t>(value);
}

void Writer::WriteInt128(__int128_t value) {
    WriteInt<__int128_t>(value);
}

void Writer::WriteString(const std::string& str) {
    EnsureCapacity(str.size());
    std::memcpy(data_.data() + offset_, str.data(), str.size());
    offset_ += str.size();
    total_ += str.size();
}

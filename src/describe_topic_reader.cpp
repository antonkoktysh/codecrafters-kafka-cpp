#include "describe_topic_reader.h"



std::vector<Topic> DescribeTopicReader::ParseTopicsArray(size_t size) const {
    std::vector<Topic> topics_array(size);
    for (auto&& topic : topics_array) {
        topic.topic_name = reader_->ReadString(reader_->ReadInt8() - 1);
        topic.topic_tag_buffer = reader_->ReadInt8();
    }
    return topics_array;
}

DescribeTopicReader::DescribeTopicReader(const std::vector<std::byte>& data)
    : reader_(std::make_unique<Reader>(data)) {
}

std::unique_ptr<Request> DescribeTopicReader::ParseRequest() {
    auto message_size = reader_->ReadInt32();
    auto header = ParseHeader();
    auto topics_array = ParseTopicsArray(static_cast<size_t>(reader_->ReadInt8() - 1));
    int32_t response_partitions_limit = reader_->ReadInt32();
    int8_t cursor = reader_->ReadInt8();
    int8_t tag_buffer = reader_->ReadInt8();
    return std::make_unique<DescribeTopicRequest>(
        message_size, header, topics_array,
        response_partitions_limit, cursor, tag_buffer);
}

RequestHeader DescribeTopicReader::ParseHeader() {
    int16_t api_key = reader_->ReadInt16();
    int16_t api_version = reader_->ReadInt16();
    int32_t correlation_id = reader_->ReadInt32();

    std::string client_id = reader_->ReadString(static_cast<size_t>(ntohs(reader_->ReadInt16())));
    int8_t tag_buffer = reader_->ReadInt8();
    return RequestHeader{api_key,        api_version,
                         correlation_id, 
                         client_id,      tag_buffer};
}

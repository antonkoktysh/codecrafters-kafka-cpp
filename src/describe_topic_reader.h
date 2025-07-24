#pragma once
#include "api_reader.h"
#include "describe_topic_request.h"
class DescribeTopicReader : public ApiReader {
public:
    ~DescribeTopicReader() = default;
    DescribeTopicReader(const std::vector<std::byte>& data)
        : reader_(std::make_unique<Reader>(data)) {
    }
    std::unique_ptr<Request> ParseRequest() override {
        int32_t message_size = reader_->ReadInt32();
        auto header = ParseHeader();
        int8_t array_len = reader_->ReadInt8();
        std::vector<Topic> topics_array(static_cast<size_t>(array_len - 1));
        for (auto&& topic : topics_array) {
            topic.topic_name_len = reader_->ReadInt8();
            topic.topic_name = reader_->ReadString(static_cast<size_t>(topic.topic_name_len - 1));
            topic.topic_tag_buffer = reader_->ReadInt8();
        }
        int32_t response_partitions_limit = reader_->ReadInt32();
        int8_t cursor = reader_->ReadInt8();
        int8_t tag_buffer = reader_->ReadInt8();
        return std::make_unique<DescribeTopicRequest>(message_size, header, array_len, topics_array,
                                                      response_partitions_limit, cursor,
                                                      tag_buffer);
    }

    RequestHeader ParseHeader() override {
        int16_t api_key = reader_->ReadInt16();
        int16_t api_version = reader_->ReadInt16();
        int32_t correlation_id = reader_->ReadInt32();

        int16_t client_id_len = reader_->ReadInt16();
        std::string client_id = reader_->ReadString(static_cast<size_t>(ntohs(client_id_len)));
        int8_t tag_buffer = reader_->ReadInt8();
        return RequestHeader{api_key,       api_version, correlation_id,
                             client_id_len, client_id,   tag_buffer};
    }

private:
    std::unique_ptr<Reader> reader_;
    size_t offset_ = 0;
};

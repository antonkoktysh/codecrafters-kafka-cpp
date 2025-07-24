#pragma once
#include "response.h"
#include "writer.h"
struct Topic {
    int8_t topic_name_len = 0;
    std::string topic_name;
    int8_t topic_tag_buffer = 0;
};

struct ResponseTopic {
    int16_t error_code = 0;
    int8_t topic_name_len = 0;
    std::string topic_name_contents;
    __int128_t topic_id = 0;
    int8_t is_internal = 0;
    int8_t partitions_array = 1;
    int32_t topic_authorized_operations = 0;
    int8_t tag_buffer = 0;
};

class DescribeTopicResponse : public Response {
public:
    DescribeTopicResponse(int32_t message_size, const ResponseHeader& header, int32_t throttle_time,
                          int8_t array_len, std::vector<ResponseTopic> topics, int8_t next_cursor,
                          int8_t tag_buffer)
        : Response(message_size, header),
          throttle_time_(throttle_time),
          array_len_(array_len),
          topics_(topics),
          next_cursor_(next_cursor),
          tag_buffer_(tag_buffer) {
    }
    ~DescribeTopicResponse() = default;
    std::vector<std::byte> ByteResponse() override {
        std::vector<std::byte> data(ntohl(message_size_) + 4);
        Writer writer(data);
        writer.WriteInt32(message_size_);

        writer.WriteInt32(header_.correlation_id);
        writer.WriteInt8(header_.tag_buffer.value());

        writer.WriteInt32(throttle_time_);

        writer.WriteInt8(array_len_);

        for (auto&& topic : topics_) {
            writer.WriteInt16(topic.error_code);
            writer.WriteInt8(topic.topic_name_len);
            writer.WriteString(topic.topic_name_contents);
            writer.WriteInt128(topic.topic_id);
            writer.WriteInt8(topic.is_internal);

            writer.WriteInt8(topic.partitions_array);
            writer.WriteInt32(topic.topic_authorized_operations);
            writer.WriteInt8(topic.tag_buffer);
        }
        writer.WriteInt8(next_cursor_);
        writer.WriteInt8(tag_buffer_);
        return data;
    }

private:
    int32_t throttle_time_ = 0;
    int8_t array_len_ = 0;
    std::vector<ResponseTopic> topics_;
    int8_t next_cursor_ = 0;
    int8_t tag_buffer_ = 0;
};

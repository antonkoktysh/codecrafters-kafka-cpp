#pragma once

#include "request.h"
#include "describe_topic_response.h"

class DescribeTopicRequest : public Request {
public:
    DescribeTopicRequest(int32_t message_size, const RequestHeader& header, int8_t array_len,
                         const std::vector<Topic> topics, int32_t response_partitions_limit,
                         int8_t cursor, int8_t tag_buffer)
        : Request(message_size, header),
          array_len_(array_len),
          topics_(topics),
          response_partitions_limit_(response_partitions_limit),
          cursor_(cursor),
          tag_buffer_(tag_buffer) {
    }
    ~DescribeTopicRequest() = default;
    std::unique_ptr<Response> Process() override {
        int32_t message_size = 0;
        ResponseHeader header(header_.correlation_id, 0);
        message_size += sizeof(int32_t) + sizeof(int8_t);
        int32_t throttle_time = 0;
        message_size += sizeof(int32_t);
        int8_t array_len = array_len_;
        message_size += sizeof(int8_t);

        std::vector<ResponseTopic> response_topics(static_cast<size_t>(array_len_ - 1));

        for (size_t ind = 0; ind < response_topics.size(); ++ind) {
            auto request_topic = topics_[ind];
            auto& response_topic = response_topics[ind];
            response_topic.error_code = htons(3);
            message_size += sizeof(response_topic.error_code);
            response_topic.topic_name_len = request_topic.topic_name_len;
            message_size += sizeof(response_topic.topic_name_len);
            response_topic.topic_name_contents = request_topic.topic_name;
            message_size += response_topic.topic_name_contents.size();
            response_topic.topic_id = 0;
            message_size += sizeof(response_topic.topic_id);
            response_topic.is_internal = 0;
            message_size += sizeof(response_topic.is_internal);
            response_topic.partitions_array = 1;
            message_size += sizeof(response_topic.partitions_array);
            response_topic.topic_authorized_operations = 0;
            message_size += sizeof(response_topic.topic_authorized_operations);
            response_topic.tag_buffer = 0;
            message_size += sizeof(response_topic.tag_buffer);
        }

        int8_t next_cursor = cursor_;
        message_size += sizeof(next_cursor);
        int8_t tag_buffer = tag_buffer_;
        message_size += sizeof(tag_buffer);

        message_size = htonl(message_size);
        return std::make_unique<DescribeTopicResponse>(message_size, header, throttle_time,
                                                       array_len, response_topics, next_cursor,
                                                       tag_buffer);
    }

private:
    int8_t array_len_ = 0;
    std::vector<Topic> topics_;
    int32_t response_partitions_limit_ = 0;
    int8_t cursor_ = 0;
    int8_t tag_buffer_ = 0;
};

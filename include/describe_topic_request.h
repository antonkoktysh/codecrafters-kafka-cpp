#pragma once

#include "request.h"
#include "describe_topic_response.h"
#include <arpa/inet.h>
class DescribeTopicRequest : public Request {
public:
    DescribeTopicRequest(int32_t message_size, const RequestHeader& header,
                         const std::vector<Topic> topics, int32_t response_partitions_limit,
                         int8_t cursor, int8_t tag_buffer);

    std::unique_ptr<Response> Process() override;

private:
    std::vector<Topic> topics_;
    int32_t response_partitions_limit_;
    int8_t cursor_;
    int8_t tag_buffer_;

    int32_t ProcessMessageSize() const;
    ResponseHeader ProcessResponseHeader() const;
    int32_t ProcessThrottleTime() const;
    int8_t ProcessNextCursor() const;
    int8_t ProcessTagBuffer() const;
    std::vector<ResponseTopic> ProcessResponseTopics() const;
    std::vector<Partition> ProcessPartitionsArray() const;
};


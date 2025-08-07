#include "describe_topic_request.h"
#include "cluster_metadata.h"
int8_t DescribeTopicRequest::ProcessNextCursor() const {
    return cursor_;
}
int8_t DescribeTopicRequest::ProcessTagBuffer() const {
    return 0;
}

ResponseHeader DescribeTopicRequest::ProcessResponseHeader() const {
    return ResponseHeader{header_.correlation_id, 0};
}
int32_t DescribeTopicRequest::ProcessThrottleTime() const {
    return 0;
}

std::vector<ResponseTopic> DescribeTopicRequest::ProcessResponseTopics() const {
    std::vector<ResponseTopic> response_topics(topics_.size());
    ClusterMetadata metadata(
        "/tmp/kraft-combined-logs/__cluster_metadata-0/00000000000000000000.log");
    for (size_t ind = 0; ind < response_topics.size(); ++ind) {
        auto request_topic = topics_[ind];
        auto& response_topic = response_topics[ind];
        response_topic.topic_name_contents = request_topic.topic_name;
        if (metadata.CountTopicByName(response_topic.topic_name_contents)) {
            response_topic.topic_id =
                metadata.FindTopicIdByName(response_topic.topic_name_contents).value();
            response_topic.error_code = htons(0);
        } else {
            response_topic.error_code = htons(3);
        }

        if (metadata.CountPartitions(response_topic.topic_id)) {
            auto to_find = metadata.FindPartitions(response_topic.topic_id).value();
            for (std::int32_t partition_id :
                 to_find) {
                response_topic.partitions_array.emplace_back(0, partition_id);
            }
        }

        response_topic.is_internal = 0;

        response_topic.topic_authorized_operations = 54;
        response_topic.tag_buffer = 0;
    }
    return response_topics;
}

std::vector<Partition> DescribeTopicRequest::ProcessPartitionsArray() const {
}

DescribeTopicRequest::DescribeTopicRequest(int32_t message_size, const RequestHeader& header,
                                           const std::vector<Topic> topics,
                                           int32_t response_partitions_limit, int8_t cursor,
                                           int8_t tag_buffer)
    : Request(message_size, header),
      topics_(topics),
      response_partitions_limit_(response_partitions_limit),
      cursor_(cursor),
      tag_buffer_(tag_buffer) {
}

int32_t DescribeTopicRequest::ProcessMessageSize() const {
    int32_t message_size = sizeof(int32_t) + sizeof(int8_t) + sizeof(int32_t) + sizeof(int8_t);
    for (const auto& topic : topics_) {
        message_size += sizeof(int16_t);
        message_size += sizeof(int8_t);
        message_size += static_cast<int32_t>(topic.topic_name.size());
        message_size += 16;
        message_size += sizeof(int8_t);
        message_size += sizeof(int8_t);
        message_size += sizeof(int32_t);
        message_size += sizeof(int8_t);
    }
    message_size += sizeof(int8_t);
    message_size += sizeof(int8_t);

    return message_size;
}

std::unique_ptr<Response> DescribeTopicRequest::Process() {
    ResponseHeader header = ProcessResponseHeader();
    int32_t throttle_time = ProcessThrottleTime();
    std::vector<ResponseTopic> response_topics = ProcessResponseTopics();
    int8_t next_cursor = ProcessNextCursor();
    int8_t tag_buffer = ProcessTagBuffer();

    int32_t message_size = htonl(ProcessMessageSize());
    return std::make_unique<DescribeTopicResponse>(message_size, header, throttle_time,
                                                   response_topics, next_cursor, tag_buffer);
}

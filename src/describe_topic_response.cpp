#include "describe_topic_response.h"

DescribeTopicResponse::DescribeTopicResponse(int32_t message_size, const ResponseHeader& header,
                                             int32_t throttle_time,
                                             std::vector<ResponseTopic> topics, int8_t next_cursor,
                                             int8_t tag_buffer)
    : Response(message_size, header),
      throttle_time_(throttle_time),

      topics_(topics),
      next_cursor_(next_cursor),
      tag_buffer_(tag_buffer) {
}

std::vector<std::byte> DescribeTopicResponse::ByteResponse() {
    std::vector<std::byte> data(1024);
    Writer writer(data, 4);
    // writer.WriteInt32(message_size_);

    writer.WriteInt32(header_.correlation_id);
    writer.WriteInt8(header_.tag_buffer.value());

    writer.WriteInt32(throttle_time_);

    writer.WriteInt8(static_cast<int8_t>(topics_.size() + 1));

    for (auto&& topic : topics_) {
        writer.WriteInt16(topic.error_code);
        writer.WriteInt8(static_cast<int8_t>(topic.topic_name_contents.size() + 1));
        writer.WriteString(topic.topic_name_contents);
        writer.WriteInt128(topic.topic_id);
        writer.WriteInt8(topic.is_internal);

        writer.WriteInt8(static_cast<int8_t>(topic.partitions_array.size() + 1));
        WritePartitions(topic, writer);
        writer.WriteInt32(htonl(topic.topic_authorized_operations));
        writer.WriteInt8(topic.tag_buffer);
    }
    writer.WriteInt8(next_cursor_);
    writer.WriteInt8(tag_buffer_);
    
    writer.WriteMessageSize(htonl(static_cast<int32_t>(writer.TotalWritten())));
    return data;
}

void DescribeTopicResponse::WritePartitions(ResponseTopic& topic, Writer& writer) {
    for (auto&& partition : topic.partitions_array) {
        writer.WriteInt16(partition.error_code);
        writer.WriteInt32(htonl(partition.partition_index));
        writer.WriteInt32(partition.leader_id);
        writer.WriteInt32(partition.leader_epoch);

        writer.WriteInt8(partition.replica_nodes.size() + 2);
        writer.WriteInt32(1);

        writer.WriteInt8(partition.isr_nodes.size() + 2);
        writer.WriteInt32(htonl(54));

        writer.WriteInt8(partition.eligible_leader_replicas.size() + 1);
        writer.WriteInt8(partition.last_known_elr.size() + 1);
        writer.WriteInt8(partition.offline_replicas.size() + 1);

        writer.WriteInt8(partition.tag_buffer);
    }
}
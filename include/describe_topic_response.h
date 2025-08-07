#pragma once
#include "response.h"
#include "writer.h"
#include <arpa/inet.h>
struct Topic {
    std::string topic_name;
    int8_t topic_tag_buffer;
};

struct Partition {
    int16_t error_code;
    int32_t partition_index;
    int32_t leader_id;
    int32_t leader_epoch;
    std::vector<std::int32_t> replica_nodes;
    std::vector<std::int32_t> isr_nodes;
    std::vector<std::int32_t> eligible_leader_replicas;
    std::vector<std::int32_t> last_known_elr;
    std::vector<std::int32_t> offline_replicas;
    int8_t tag_buffer;
};

struct ResponseTopic {
    int16_t error_code;
    std::string topic_name_contents;
    __int128_t topic_id;
    int8_t is_internal;
    std::vector<Partition> partitions_array;
    int32_t topic_authorized_operations;
    int8_t tag_buffer;
};

class DescribeTopicResponse : public Response {
public:
    DescribeTopicResponse(int32_t message_size, const ResponseHeader& header, int32_t throttle_time,
                          std::vector<ResponseTopic> topics, int8_t next_cursor, int8_t tag_buffer);
    std::vector<std::byte> ByteResponse() override;

private:
    void WritePartitions(ResponseTopic& topic, Writer& writer);
    int32_t throttle_time_;
    std::vector<ResponseTopic> topics_;
    int8_t next_cursor_;
    int8_t tag_buffer_;
};

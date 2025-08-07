#pragma once
#include <cstdint>
#include <vector>
#include "record.h"


struct RecordBatch {
     std::int64_t base_offset;
        std::int32_t batch_length;
        std::int32_t partition_leader_epoch;
        std::int8_t magic_byte;
        std::int32_t crc;
        std::int16_t attributes;
        std::int32_t last_offset_delta;
        std::int64_t base_timestamp;
        std::int64_t max_timestamp;
        std::int64_t producer_id;
        std::int16_t producer_epoch;
        std::int32_t base_sequence;
        std::vector<Record> records;
};
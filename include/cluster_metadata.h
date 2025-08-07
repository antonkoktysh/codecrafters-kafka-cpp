#pragma once

#include <string>
#include "record.h"
#include "record_batch.h"
#include <map>
#include <cstdint>
#include <optional>
#include <reader.h>
#include <iomanip>
#include <cstddef>  // для std::byte
#include <fstream>
#include <iostream>

class ClusterMetadata {
public:

    explicit ClusterMetadata(const std::string& log_file_path) : reader_(log_file_path) {
        while (reader_.Remaining() > 0) {
            record_bathces_.emplace_back(ReadRecordBatch());
        }
        Logg();
    }

    std::string int128_to_string(__int128_t value) {
    if (value == 0) return "0";
    
    bool is_negative = false;
    __uint128_t abs_value;
    
    if (value < 0) {
        is_negative = true;
        abs_value = static_cast<__uint128_t>(-value);
    } else {
        abs_value = static_cast<__uint128_t>(value);
    }
    
    std::string result;
    while (abs_value > 0) {
        result = char('0' + abs_value % 10) + result;
        abs_value /= 10;
    }
    
    if (is_negative) {
        result = "-" + result;
    }
    return result;
}

void Logg() {
    std::cout << "\n\nIn Logger now\n\n";

    // Вывод id_name_
    std::cout << "id_name_ map:\n";
    for (const auto& [id, name] : id_name_) {
        std::cout << "  " << int128_to_string(id) << " -> \"" << name << "\"\n";
    }
    std::cout << "(" << id_name_.size() << " elements)\n\n";

    // Вывод id_partitions_
    std::cout << "id_partitions_ map:\n";
    for (const auto& [id, vec] : id_partitions_) {
        std::cout << "  " << int128_to_string(id) << " -> [";
        for (size_t i = 0; i < vec.size(); ++i) {
            std::cout << vec[i];
            if (i < vec.size() - 1) std::cout << ", ";
        }
        std::cout << "]\n";
    }
    std::cout << "(" << id_partitions_.size() << " elements)\n\n";

    // Вывод name_id_
    std::cout << "name_id_ map:\n";
    for (const auto& [name, id] : name_id_) {
        std::cout << "  \"" << name << "\" -> " << int128_to_string(id) << "\n";
    }
    std::cout << "(" << name_id_.size() << " elements)\n\n";
}

   // Единая функция проверки существования элемента
bool CountTopic(__int128_t topic_id) const {
    return id_name_.contains(topic_id); // C++20 style
}

// Для совместимости с C++17
bool CountTopicByName(const std::string& name) const {
    return name_id_.find(name) != name_id_.end();
}

bool CountPartitions(__int128_t topic_id) const { // Исправлено имя
    return id_partitions_.find(topic_id) != id_partitions_.end();
}

std::optional<std::string> FindTopicName(__int128_t topic_id) {
    if (auto it = id_name_.find(topic_id); it != id_name_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<__int128_t> FindTopicIdByName(const std::string& topic_name) {
    if (auto it = name_id_.find(topic_name); it != name_id_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<const std::vector<std::int32_t>> FindPartitions(__int128_t topic_id) {
    if (auto it = id_partitions_.find(topic_id); it != id_partitions_.end()) {
        return it->second;
    }
    return std::nullopt;
}

private:
    Reader reader_;
    std::vector<RecordBatch> record_bathces_;
    std::map<__int128_t, std::string> id_name_;
    std::map<__int128_t, std::vector<std::int32_t>> id_partitions_;
    std::map<std::string, __int128_t> name_id_;
    std::vector<std::byte> ReadRecordValue() {
        int32_t value_len = reader_.ReadVarint();
        std::vector<std::byte> value(static_cast<size_t>(value_len));
        reader_.Read(value.data(), value.size());
        return value;
    }


    Record ReadRecord() {
        int8_t length = reader_.ReadVarint();
        auto attributes = reader_.ReadInt8();
        int8_t timestamp_delta = reader_.ReadInt8();
        int8_t offset_delta = reader_.ReadInt8();
        reader_.ReadInt8();

        auto value = ReadRecordValue();
        // std::cout << "HedDump of RecordValue" << std::endl;
        // std::cout << std::hex;
        // for (std::byte b : value) {
        //     std::cout << std::setw(2) << std::setfill('0') << static_cast<int>(b) << " ";
        // }
        // std::cout << std::dec << "\n";
        ParseRecordValue(value);
        auto headers_count = reader_.ReadInt8();
        return Record{length, attributes, timestamp_delta, offset_delta, value};
    }

    RecordBatch ReadRecordBatch() {
        RecordBatch record_batch;
        record_batch.base_offset = reader_.ReadInt64();
        record_batch.batch_length = reader_.ReadInt32();
        record_batch.partition_leader_epoch = reader_.ReadInt32();
        record_batch.magic_byte = reader_.ReadInt8();
        record_batch.crc = reader_.ReadInt32();
        record_batch.attributes = reader_.ReadInt16();
        record_batch.last_offset_delta = reader_.ReadInt32();
        record_batch.base_timestamp = reader_.ReadInt64();
        record_batch.max_timestamp = reader_.ReadInt64();
        record_batch.producer_id = reader_.ReadInt64();
        record_batch.producer_epoch = reader_.ReadInt16();
        record_batch.base_sequence = reader_.ReadInt32();
        auto records_length = reader_.ReadInt32();
        std::vector<Record> records(ntohl(records_length));
        for (auto&& record : records) {
            record = ReadRecord();
        }
        return record_batch;
    }

    void ParseRecordValue(std::vector<std::byte>& value) {
        Reader reader(value);

        std::int8_t frame_version = reader.ReadInt8();

        std::int8_t type = reader.ReadInt8();
        std::int8_t version = reader.ReadInt8();
        switch (static_cast<size_t>(type)) {
            case 2: {
                auto topic_name = reader.ReadString(reader.ReadInt8() - 1);
                auto topic_id = reader.ReadInt128();
                id_name_[topic_id] = topic_name;
                name_id_[topic_name] = topic_id;
                break;
            }
            case 3: {
                auto partition_id = reader.ReadInt32();
                std::cout << "PART_IND" << partition_id << " " << (partition_id) << std::endl;
                auto topic_id = reader.ReadInt128();
                id_partitions_[topic_id].push_back(ntohl(partition_id));
                break;
            }
            case 12:
                break;
            default:
                throw std::runtime_error("unexpected");
        }
    }
};
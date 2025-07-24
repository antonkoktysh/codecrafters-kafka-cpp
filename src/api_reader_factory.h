#pragma once
#include "api_versions_reader.h"
#include "describe_topic_reader.h"
class ApiReaderFactory {
public:
    std::unique_ptr<ApiReader> Create(const std::vector<std::byte>& data) {
        int16_t api_key;
        std::memcpy(&api_key, data.data() + sizeof(int32_t), sizeof(int16_t));
        switch (ntohs(api_key)) {
            case 18:
                return std::make_unique<ApiVersionsReader>(data);
            case 75:
                return std::make_unique<DescribeTopicReader>(data);
            default:
                throw std::runtime_error("dsadas");
        }
    }
};

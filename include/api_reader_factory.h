#pragma once
#include "api_versions_reader.h"
#include "describe_topic_reader.h"
class ApiReaderFactory {
public:
    virtual std::unique_ptr<ApiReader> Create(const std::vector<std::byte>& data) = 0;
    virtual ~ApiReaderFactory() = default;
};

class ApiVersionsReaderFactory : public ApiReaderFactory {
public:
    std::unique_ptr<ApiReader> Create(const std::vector<std::byte>& data) override {
        return std::make_unique<ApiVersionsReader>(data);
    }
};

class DescribeTopicReaderFactory : public ApiReaderFactory {
public:
    std::unique_ptr<ApiReader> Create(const std::vector<std::byte>& data) override {
        return std::make_unique<DescribeTopicReader>(data);
    }
};

class ReaderFactoryCreator {
public:
    static std::unique_ptr<ApiReaderFactory> CreateFactory(const std::vector<std::byte>& data) {
        if (data.size() < sizeof(int32_t) + sizeof(int16_t)) {
            throw std::runtime_error("Invalid data size");
        }

        int16_t api_key;
        std::memcpy(&api_key, data.data() + sizeof(int32_t), sizeof(int16_t));
        api_key = ntohs(api_key);

        switch (api_key) {
            case 18:
                return std::make_unique<ApiVersionsReaderFactory>();
            case 75:
                return std::make_unique<DescribeTopicReaderFactory>();
            default:
                throw std::runtime_error("Unsupported API key: " + std::to_string(api_key));
        }
    }
};

#pragma once
#include "api_reader.h"
#include "describe_topic_request.h"
#include <arpa/inet.h>
class DescribeTopicReader : public ApiReader {
public:
    DescribeTopicReader(const std::vector<std::byte>& data);
    std::unique_ptr<Request> ParseRequest() override;

    RequestHeader ParseHeader() override;

private:
    std::unique_ptr<Reader> reader_;
    std::vector<Topic> ParseTopicsArray(size_t size) const;
};


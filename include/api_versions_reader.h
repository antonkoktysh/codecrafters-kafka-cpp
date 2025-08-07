#pragma once
#include "api_reader.h"
#include <arpa/inet.h>
class ApiVersionsReader : public ApiReader {
public:
    ApiVersionsReader(const std::vector<std::byte>& data);
    std::unique_ptr<Request> ParseRequest() override;

    RequestHeader ParseHeader() override;

private:
    std::unique_ptr<Reader> reader_;
};


#pragma once
#include "reader.h"
#include "api_versions_request.h"
class ApiReader {
public:
    virtual ~ApiReader() = default;
    virtual std::unique_ptr<Request> ParseRequest() = 0;
    virtual RequestHeader ParseHeader() = 0;
};

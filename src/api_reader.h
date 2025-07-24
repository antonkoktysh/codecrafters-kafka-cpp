#pragma once
#include "reader.h"

class ApiReader {
public:
    virtual ~ApiReader() = default;
    virtual std::unique_ptr<Request> ParseRequest() {
    }
    virtual RequestHeader ParseHeader() {
    }
};

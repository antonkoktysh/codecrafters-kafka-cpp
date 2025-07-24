#pragma once
#include "api_reader.h"
class ApiVersionsReader : public ApiReader {
public:
    ~ApiVersionsReader() = default;
    ApiVersionsReader(const std::vector<std::byte>& data)
        : reader_(std::make_unique<Reader>(data)) {
    }
    std::unique_ptr<Request> ParseRequest() override {
        int32_t message_size = reader_->ReadInt32();
        auto header = ParseHeader();
        int8_t client_id_len = reader_->ReadInt8();
        std::string client_id_contents =
            reader_->ReadString(static_cast<size_t>(ntohs(client_id_len - 1)));

        int8_t client_software_version_len = reader_->ReadInt8();
        std::string client_software_version =
            reader_->ReadString(static_cast<size_t>(ntohs(client_software_version_len - 1)));
        int8_t tag_buffer = reader_->ReadInt8();

        return std::make_unique<ApiVersionsRequest>(message_size, header, client_id_len,
                                                    client_id_contents, client_software_version_len,
                                                    client_software_version, tag_buffer);
    }

    RequestHeader ParseHeader() override {
        int16_t api_key = reader_->ReadInt16();
        int16_t api_version = reader_->ReadInt16();
        int32_t correlation_id = reader_->ReadInt32();

        int16_t client_id_len = reader_->ReadInt16();
        std::string client_id = reader_->ReadString(static_cast<size_t>(ntohs(client_id_len)));
        int8_t tag_buffer = reader_->ReadInt8();
        return RequestHeader{api_key,       api_version, correlation_id,
                             client_id_len, client_id,   tag_buffer};
    }

private:
    std::unique_ptr<Reader> reader_;
    size_t offset_ = 0;
};

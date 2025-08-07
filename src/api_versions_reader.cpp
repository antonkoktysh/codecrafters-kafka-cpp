#include "api_versions_reader.h"

ApiVersionsReader::ApiVersionsReader(const std::vector<std::byte>& data)
    : reader_(std::make_unique<Reader>(data)) {
}

std::unique_ptr<Request> ApiVersionsReader::ParseRequest() {
    int32_t message_size = reader_->ReadInt32();
    auto header = ParseHeader();

    std::string client_id_contents =
        reader_->ReadString(static_cast<size_t>(ntohs(reader_->ReadInt8()) - 1));

    std::string client_software_version =
        reader_->ReadString(static_cast<size_t>(ntohs(reader_->ReadInt8()) - 1));
    int8_t tag_buffer = reader_->ReadInt8();

    return std::make_unique<ApiVersionsRequest>(
        message_size, header, 
        client_id_contents,
        client_software_version, tag_buffer);
}

RequestHeader ApiVersionsReader::ParseHeader() {
    int16_t api_key = reader_->ReadInt16();
    int16_t api_version = reader_->ReadInt16();
    int32_t correlation_id = reader_->ReadInt32();

    std::string client_id = reader_->ReadString(static_cast<size_t>(reader_->ReadInt16()));
    int8_t tag_buffer = reader_->ReadInt8();
    return RequestHeader{api_key,        api_version,
                         correlation_id,
                         client_id,      tag_buffer};
}

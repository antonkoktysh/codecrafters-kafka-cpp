#pragma once
#include "api_versions_response.h"
#include "request.h"
class ApiVersionsRequest : public Request {
public:
    ApiVersionsRequest(int32_t message_size, const RequestHeader& header, int8_t client_id_len,
                       const std::string& client_id_contents, int8_t client_software_version_len,
                       const std::string& client_software_version_contents, int8_t tag_buffer)
        : Request(message_size, header),
          client_id_len_(client_id_len),
          client_id_contents_(client_id_contents),
          client_software_version_len_(client_software_version_len),
          client_software_version_contents_(client_software_version_contents),
          tag_buffer_(tag_buffer) {
    }
    std::unique_ptr<Response> Process() override {
        int32_t message_size = 0;
        ResponseHeader header;
        header.correlation_id = header_.correlation_id;
        message_size += sizeof(int32_t);
        int16_t error_code = header_.api_version == ntohs(4) ? htons(0) : htons(35);
        message_size += sizeof(int16_t);
        std::vector<ApiVersion> api_versions;
        api_versions.emplace_back(htons(18), htons(1), htons(4), 0);
        api_versions.emplace_back(htons(75), htons(0), htons(0), 0);
        int8_t array_len = static_cast<int8_t>(api_versions.size() + 1);
        //
        message_size += sizeof(int8_t);
        message_size +=
            (3 * sizeof(int16_t) + sizeof(int8_t)) * static_cast<int32_t>(api_versions.size());
        int32_t throttle_time = 0;
        message_size += sizeof(int32_t);

        int8_t tag_buffer = 0;
        message_size += sizeof(int8_t);
        message_size = htonl(message_size);
        return std::make_unique<ApiVersionsResponse>(message_size, header, error_code, array_len,
                                                     api_versions, throttle_time, tag_buffer);
    }

private:
    int8_t client_id_len_ = 0;
    std::string client_id_contents_;
    int8_t client_software_version_len_ = 0;
    std::string client_software_version_contents_;
    int8_t tag_buffer_ = 0;
};

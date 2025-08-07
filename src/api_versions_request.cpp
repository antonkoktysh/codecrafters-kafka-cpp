#include "api_versions_request.h"

ApiVersionsRequest::ApiVersionsRequest(int32_t message_size, const RequestHeader& header,
                                      const std::string& client_id_contents,
                                       const std::string& client_software_version_contents,
                                       int8_t tag_buffer)
    : Request(message_size, header),
      client_id_contents_(client_id_contents),
      client_software_version_contents_(client_software_version_contents),
      tag_buffer_(tag_buffer) {
}

ResponseHeader ApiVersionsRequest::ProcessResponseHeader() const {
    return ResponseHeader{header_.correlation_id};
}
int16_t ApiVersionsRequest::ProcessErrorCode() const {
    return header_.api_version == ntohs(4) ? htons(0) : htons(35);
}

std::vector<ApiVersion> ApiVersionsRequest::ParseApiVersions() const {
    std::vector<ApiVersion> api_versions;
    api_versions.emplace_back(htons(18), htons(1), htons(4), 0);
    api_versions.emplace_back(htons(75), htons(0), htons(0), 0);
    return api_versions;
}
int32_t ApiVersionsRequest::ProcessThrottleTime() const {
    return 0;
}
int8_t ApiVersionsRequest::ProcessTagBuffer() const {
    return 0;
}

int32_t ApiVersionsRequest::CalculateMessageSize() const {
    int32_t message_size =
        sizeof(int32_t) + sizeof(int16_t) + sizeof(int8_t) + sizeof(int32_t) + sizeof(int8_t);

    message_size +=
        (3 * sizeof(int16_t) + sizeof(int8_t)) * static_cast<int32_t>(ParseApiVersions().size());

    return message_size;
}
std::unique_ptr<Response> ApiVersionsRequest::Process() {
    ResponseHeader header = ProcessResponseHeader();
    int16_t error_code = ProcessErrorCode();
    std::vector<ApiVersion> api_versions = ParseApiVersions();
    int32_t throttle_time = ProcessThrottleTime();
    int8_t tag_buffer = ProcessTagBuffer();
    int32_t message_size = htonl(CalculateMessageSize());
    return std::make_unique<ApiVersionsResponse>(message_size, header, error_code, api_versions,
                                                 throttle_time, tag_buffer);
}

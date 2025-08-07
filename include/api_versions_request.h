#pragma once
#include "api_versions_response.h"
#include "request.h"
#include <arpa/inet.h>
class ApiVersionsRequest : public Request {
public:
    ApiVersionsRequest(int32_t message_size, const RequestHeader& header,
                       const std::string& client_id_contents,
                       const std::string& client_software_version_contents, int8_t tag_buffer);
    std::unique_ptr<Response> Process() override;

private:
    int32_t CalculateMessageSize() const;
    ResponseHeader ProcessResponseHeader() const;
    std::vector<ApiVersion> ParseApiVersions() const;
    int16_t ProcessErrorCode() const;
    int32_t ProcessThrottleTime() const;
    int8_t ProcessTagBuffer() const;
    std::string client_id_contents_;
    std::string client_software_version_contents_;
    int8_t tag_buffer_;
};


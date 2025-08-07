#pragma once
#include "response.h"
#include "writer.h"
#include <arpa/inet.h>
struct ApiVersion {
    int16_t api_key;
    int16_t min_supported_api_key;
    int16_t max_supported_api_key;
    int8_t tag_buffer;
};

class ApiVersionsResponse : public Response {
public:
    ApiVersionsResponse(int32_t message_size, const ResponseHeader& header, int16_t error_code,
                        const std::vector<ApiVersion> api_versions, int32_t throttle_time,
                        int8_t tag_buffer);
    std::vector<std::byte> ByteResponse() override;

private:
    int16_t error_code_;
    std::vector<ApiVersion> api_versions_;
    int32_t throttle_time_;
    int8_t tag_buffer_;
};


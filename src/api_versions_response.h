#pragma once
#include "response.h"
#include "writer.h"
struct ApiVersion {
    int16_t api_key = 0;
    int16_t min_supported_api_key = 0;
    int16_t max_supported_api_key = 0;
    int8_t tag_buffer = 0;
};

class ApiVersionsResponse : public Response {
public:
    ApiVersionsResponse(int32_t message_size, const ResponseHeader& header, int16_t error_code,
                        int8_t array_len, const std::vector<ApiVersion> api_versions,
                        int32_t throttle_time, int8_t tag_buffer)
        : Response(message_size, header),
          error_code_(error_code),
          array_len_(array_len),
          api_versions_(api_versions),
          throttle_time_(throttle_time),
          tag_buffer_(tag_buffer) {
    }
    std::vector<std::byte> ByteResponse() override {
        std::vector<std::byte> data(ntohl(message_size_) + sizeof(int32_t));
        Writer writer(data);

        writer.WriteInt32(message_size_);
        writer.WriteInt32(header_.correlation_id);
        writer.WriteInt16(error_code_);
        writer.WriteInt8(array_len_);

        for (size_t ind = 0; ind < api_versions_.size(); ++ind) {
            writer.WriteInt16(api_versions_[ind].api_key);
            writer.WriteInt16(api_versions_[ind].min_supported_api_key);
            writer.WriteInt16(api_versions_[ind].max_supported_api_key);
            writer.WriteInt8(api_versions_[ind].tag_buffer);
        }

        writer.WriteInt32(throttle_time_);
        writer.WriteInt8(tag_buffer_);

        return data;
    }

private:
    int16_t error_code_ = 0;
    int8_t array_len_ = 0;
    std::vector<ApiVersion> api_versions_;
    int32_t throttle_time_ = 0;
    int8_t tag_buffer_ = 0;
};

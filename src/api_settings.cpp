#include <cstdint>

enum class KafkaErrorCode : int16_t {
  NONE = 0,
  UNSUPPORTED_VERSION = 35,
  INVALID_REQUEST = 42,
  UNKNOWN_SERVER_ERROR = 48,
};

enum class KafkaApiKey : int16_t {
  PRODUCE = 0,
  FETCH = 1,
  LIST_OFFSETS = 2,
  METADATA = 3,
  LEADER_AND_ISR = 4,
  STOP_REPLICA = 5,
  API_VERSIONS = 18,
};

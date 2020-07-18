#pragma once

#include <sstream>

namespace msw::helpers {

class OstreamLogging {
  std::ostringstream message_;
 public:
  template<typename T>
  OstreamLogging& operator<<(const T& data) {
    message_ << data;
    return *this;
  }

  std::string as_str() const { return message_.str(); }
};

}  // namespace msw::helpers

// #define ____INTERNAL_OstreamLoggingEXPAND_endInternal(other) (msw::helpers::OstreamLogging() << complex).as_str()
#define ____INTERNAL_SPDLOG_OSR_ANY_endInternal(lvl, other) SPDLOG_##lvl((msw::helpers::OstreamLogging() << other).as_str())

// #define SPDLOG_OSR_DEBUG(complex) SPDLOG_DEBUG()
#define SPDLOG_OSR_DEBUG(complex) ____INTERNAL_SPDLOG_OSR_ANY_endInternal(DEBUG, complex)
#define SPDLOG_OSR_WARN(complex) ____INTERNAL_SPDLOG_OSR_ANY_endInternal(WARN, complex)
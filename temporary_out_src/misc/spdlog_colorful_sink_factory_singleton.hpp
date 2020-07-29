#pragma once

#include <spdlog/logger.h>
#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace spdl::internal {

class ColorfulSinkFactorySingleton {
  static ColorfulSinkFactorySingleton *inst();

  static std::unique_ptr<spdl::internal::ColorfulSinkFactorySingleton> make_the_inst();

 protected:

  virtual spdlog::sink_ptr get_sink() = 0;

  ColorfulSinkFactorySingleton(ColorfulSinkFactorySingleton &&) = default;
  ColorfulSinkFactorySingleton &operator=(ColorfulSinkFactorySingleton &&) = default;
  ColorfulSinkFactorySingleton() = default;

 public:
  static spdlog::sink_ptr make_colorful_stdout();

  ColorfulSinkFactorySingleton(ColorfulSinkFactorySingleton const &) = delete;
  ColorfulSinkFactorySingleton &operator=(ColorfulSinkFactorySingleton const &) = delete;
  virtual ~ColorfulSinkFactorySingleton() = default;
};
}  // namespace spdl::internal

#pragma once
#include <string>
#include <src/protobufs/app_config.pb.h>

namespace msw::model {
class AppConfig {

  msw_proto_cfg::PlaycntConfig proto_app_config_;


  AppConfig() = default;
  ~AppConfig() = default;
  AppConfig(const AppConfig&) = delete;
  AppConfig& operator=(const AppConfig&) = delete;
public:
  static AppConfig& instance() {
    static AppConfig instance;
    return instance;
  }

  void set_library_path(const std::string& path);
  void set_file_to_listen(const std::string& pathFileToListen);
  void store_to_file(const char* str);

};
} // namespace msw::model

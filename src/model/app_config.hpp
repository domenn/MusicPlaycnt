#pragma once
#include <src/protobufs/app_config.pb.h>

#include <string>

#include "serializable.hpp"

namespace msw::model {
class AppConfig : public Serializable {
  msw_proto_cfg::PlaycntConfig proto_app_config_{};

 public:
  static AppConfig& instance() {
    static AppConfig instance;
    return instance;
  }
  AppConfig() : Serializable(proto_app_config_) {}

  ~AppConfig() = default;
  
  static std::string get_path_to_config_file();
  AppConfig(const AppConfig&) = delete;
  AppConfig& operator=(const AppConfig&) = delete;
  AppConfig(AppConfig&&) noexcept ;

  void set_library_path(const std::string& path);
  void set_file_to_listen(const std::string& pathFileToListen);
  const std::string& file_to_listen() const;
};
}  // namespace msw::model

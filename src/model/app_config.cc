#include "app_config.hpp"
#include "src/misc/consts.hpp"
#include "src/misc/utilities.hpp"

void msw::model::AppConfig::set_library_path(const std::string& path) { proto_app_config_.set_librarypath(path); }

void msw::model::AppConfig::set_file_to_listen(const std::string& pathFileToListen) {
  proto_app_config_.set_filetolisten(pathFileToListen);
}

const std::string& msw::model::AppConfig::file_to_listen() const {
  return proto_app_config_.filetolisten();
}

std::string msw::model::AppConfig::get_path_to_config_file() {
  return msw::helpers::Utilities::file_in_app_folder(msw::consts::CONFIG_FILENAME);
}

msw::model::AppConfig::AppConfig(msw::model::AppConfig&& other) noexcept
    : Serializable(proto_app_config_), proto_app_config_(std::move(other.proto_app_config_)) {}
// msw::model::AppConfig msw::model::AppConfig::from_file() {
//  AppConfig app_config;
//  return msw::model::AppConfig(model::AppConfig());
//}

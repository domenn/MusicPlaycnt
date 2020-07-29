#include "app_config.hpp"

void msw::model::AppConfig::set_library_path(const std::string& path) {
  proto_app_config_.set_librarypath(path);
}

void msw::model::AppConfig::set_file_to_listen(const std::string& pathFileToListen) {
  proto_app_config_.set_filetolisten(pathFileToListen);
}

void msw::model::AppConfig::store_to_file(const char* str) {

}

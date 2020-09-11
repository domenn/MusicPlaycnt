#include "app_config.hpp"

#include <sago/platform_folders.h>
#include <src/misc/consts.hpp>
#include <src/misc/utilities.hpp>

void msw::model::AppConfig::set_library_path(const std::string& path) { proto_app_config_.set_librarypath(path); }

void msw::model::AppConfig::set_stored_state(const std::string& path) { proto_app_config_.set_storedstate(path); }

void msw::model::AppConfig::set_file_to_listen(const std::string& pathFileToListen) {
  proto_app_config_.set_filetolisten(pathFileToListen);
}

const std::string& msw::model::AppConfig::file_to_listen() const { return proto_app_config_.filetolisten(); }

const int32_t msw::model::AppConfig::playcount_threshold_seconds() const {
  return proto_app_config_.playcountthresholdseconds();
}

const std::string& msw::model::AppConfig::library_path() const { return proto_app_config_.librarypath(); }

const std::string& msw::model::AppConfig::stored_state_path() const { return proto_app_config_.storedstate(); }

std::vector<std::string> msw::model::AppConfig::delimiters() const {
  const auto d_size = proto_app_config_.delimiters_size();
  std::vector<std::string> returns;
  returns.reserve(proto_app_config_.delimiters_size());
  for (auto delimiter : proto_app_config_.delimiters()) {
    returns.emplace_back(std::move(delimiter));
  }
  return returns;
}

const std::string& msw::model::AppConfig::DelimiterIterable::operator*() const {
  return app_config_->proto_app_config_.delimiters(idx_);
}

const std::string* msw::model::AppConfig::DelimiterIterable::operator->() const {
  return &app_config_->proto_app_config_.delimiters(idx_);
}

msw::model::AppConfig::DelimiterIterable msw::model::AppConfig::iterate_delimiters() const {
  return DelimiterIterable(this);
}

const std::string& msw::model::AppConfig::stored_data() const { return proto_app_config_.storeddata(); }

const std::string& msw::model::AppConfig::song_print_format() const {
  const auto& spf = proto_app_config_.songprintformat();
  if (spf.empty()) {
    const_cast<AppConfig*>(this)->set_song_print_format(consts::DEFAULT_OSTREAM_SONG);
  }
  return spf;
}

void msw::model::AppConfig::set_stored_data(const std::string& storeddata) {
  proto_app_config_.set_storeddata(storeddata);
}

void msw::model::AppConfig::set_playcount_threshold_seconds(int32_t seconds) {
  proto_app_config_.set_playcountthresholdseconds(seconds);
}

void msw::model::AppConfig::set_song_print_format(const std::string& fmt) {
  proto_app_config_.set_songprintformat(fmt);
}

msw::model::AppConfig::DelimiterIterable msw::model::AppConfig::DelimiterIterable::begin() const {
  return DelimiterIterable{app_config_, 0};
}

msw::model::AppConfig::DelimiterIterable msw::model::AppConfig::DelimiterIterable::end() const {
  return DelimiterIterable{app_config_, app_config_->proto_app_config_.delimiters_size()};
}

void msw::model::AppConfig::set_delimiters(std::vector<std::string> delimiters) {
  for (auto&& delimiter : delimiters) {
    proto_app_config_.add_delimiters(std::move(delimiter));
  }
}

msw::model::AppConfig& msw::model::AppConfig::set_my_defaults() {
  set_file_to_listen(sago::getMusicFolder() + "/foo_np_log.txt");
  set_library_path(sago::getMusicFolder() + "/my_music");
  set_delimiters({";", ": ", " ;; ", " ;; ", " ;;; "});
  set_stored_state(msw::helpers::Utilities::file_in_app_folder(consts::DEFAULT_STATE_FILE_NAME));
  set_stored_data(msw::helpers::Utilities::file_in_app_folder(consts::DEFAULT_DATA_FILE_NAME));
  set_playcount_threshold_seconds(45);
  set_song_print_format(consts::DEFAULT_OSTREAM_SONG);
  return *this;
}

void msw::model::AppConfig::save_me() {
  const auto path = msw::helpers::Utilities::file_in_app_folder_with_creating_app_folder(msw::consts::CONFIG_FILENAME);
  serialize(path);
}

std::string msw::model::AppConfig::get_path_to_config_file() {
  return msw::helpers::Utilities::file_in_app_folder(msw::consts::CONFIG_FILENAME);
}

msw::model::AppConfig::AppConfig(msw::model::AppConfig&& other) noexcept
    : Serializable(&proto_app_config_), proto_app_config_(std::move(other.proto_app_config_)) {}

// msw::model::AppConfig msw::model::AppConfig::from_file() {
//  AppConfig app_config;
//  return msw::model::AppConfig(model::AppConfig());
//}

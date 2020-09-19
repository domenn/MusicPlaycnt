#pragma once
#include <src/win/windows_headers.hpp>

#include "serializable.hpp"
#include <src/protobufs/app_config.pb.h>
#include <string>

namespace msw::model {

class AppConfig : public Serializable {
  msw_proto_cfg::PlaycntConfig proto_app_config_{};

 public:
  class DelimiterIterable {
    const AppConfig* app_config_;
    int idx_{};

   public:
    DelimiterIterable(const AppConfig* app_config) : app_config_(app_config) {}

    DelimiterIterable(const AppConfig* app_config, int idx) : app_config_(app_config), idx_(idx) {}

    DelimiterIterable begin() const;
    DelimiterIterable end() const;

    DelimiterIterable& operator++() {
      ++idx_;
      return *this;
    }

    DelimiterIterable operator++(int) {
      DelimiterIterable retval = *this;
      ++(*this);
      return retval;
    }

    bool operator==(DelimiterIterable other) const { return idx_ == other.idx_; }
    bool operator!=(DelimiterIterable other) const { return !(*this == other); }
    const std::string& operator*() const;
    const std::string* operator->() const;
    // iterator traits
    using difference_type = int;
    using value_type = const std::string;
    using pointer = const std::string*;
    using reference = const std::string&;
    using iterator_category = std::forward_iterator_tag;
  };

  AppConfig() : Serializable(&proto_app_config_) {}

  ~AppConfig() = default;
  void set_delimiters(std::vector<std::string> delimiters);
  AppConfig& set_my_defaults();
  void save_me();

  static std::string get_path_to_config_file();
  AppConfig(const AppConfig&) = delete;
  AppConfig& operator=(const AppConfig&) = delete;
  AppConfig(AppConfig&&) noexcept;

  void set_library_path(const std::string& path);
  void set_stored_state(const std::string& path);
  void set_file_to_listen(const std::string& pathFileToListen);
  void set_stored_data(const std::string& path);
  void set_playcount_threshold_seconds(int32_t seconds);
  void set_song_print_format(const std::string& fmt);
  const std::string& file_to_listen() const;
  int32_t playcount_threshold_seconds() const;
  int32_t playcount_threshold_ms() const { return playcount_threshold_seconds() * 1000; }
  const std::string& library_path() const;
  const std::string& stored_state_path() const;
  const std::string& stored_data() const;
  const std::string& song_print_format() const;
  std::vector<std::string> delimiters() const;
  DelimiterIterable iterate_delimiters() const;
};
}  // namespace msw::model

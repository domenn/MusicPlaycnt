#pragma once
#include <src/protobufs/app_config.pb.h>

#include <string>

#include "serializable.hpp"

namespace msw::model {

// class MyStringIterator {
//  int num = 0;
// public:
//  MyStringIterator(long _num = 0)
//    : num(_num) {
//  }
//
//  MyStringIterator& operator++() {
//    ++num;
//    return *this;
//  }
//
//  MyStringIterator operator++(int) {
//    MyStringIterator retval = *this;
//    ++(*this);
//    return retval;
//  }
//
//  bool operator==(MyStringIterator other) const { return num == other.num; }
//  bool operator!=(MyStringIterator other) const { return !(*this == other); }
//  long operator*() { return num; }
//  // iterator traits
//  using difference_type = int;
//  using value_type = const std::string;
//  using pointer = const std::string*;
//  using reference = const std::string&;
//  using iterator_category = std::forward_iterator_tag;
//};

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

  // static AppConfig& instance() {
  //  static AppConfig instance;
  //  return instance;
  //}

  AppConfig() : Serializable(&proto_app_config_) {}

  ~AppConfig() = default;
  void set_delimiters(std::vector<std::string> delimiters);
  AppConfig& set_my_defaults();

  static std::string get_path_to_config_file();
  AppConfig(const AppConfig&) = delete;
  AppConfig& operator=(const AppConfig&) = delete;
  AppConfig(AppConfig&&) noexcept;

  void set_library_path(const std::string& path);
  void set_stored_state(const std::string& path);
  void set_file_to_listen(const std::string& pathFileToListen);
  const std::string& file_to_listen() const;
  const std::string& stored_state_path() const;
  std::vector<std::string> delimiters() const;
  DelimiterIterable iterate_delimiters() const;
};
}  // namespace msw::model

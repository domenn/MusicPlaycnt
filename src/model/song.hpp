#pragma once

#include <src/win/windows_headers.hpp>

#include "../protobufs/songs.pb.h"
#include "serializable.hpp"
#include <optional>

namespace msw::model {
class Song : public Serializable {
  std::optional<msw_proto_song::Song> proto_song_containing_optional_{};
public:
  Song(msw_proto_song::Song song);
  Song(msw_proto_song::Song * song);

  static Song deserialize(const std::string& contents);
  static Song deserialize_from_file(const std::string& file_on_disk);

  static void testing_create_restore();
  void set_artist(std::string&& artist);
  void set_album(std::string&& album);
  void set_title(std::string&& title);
  void set_path(std::string&& path);
  const std::string& album() const;
};
} // namespace msw::model

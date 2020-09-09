#pragma once

#include <optional>

#include "../protobufs/songs.pb.h"
#include "serializable.hpp"

namespace msw {
namespace helpers {
typedef std::tuple<std::string, std::string, std::string, std::string> AsTupleCmdSongItems;

/**
 * Order of items is adapted to foobar file parser.
 */
struct ParseSongItems {
  std::string artist;
  std::string album;
  std::string title;
  std::string path;

  /**
   * \return my items in the order defined in model of song / protobuf.
   */
  AsTupleCmdSongItems to_tuple() {
    return std::make_tuple(std::move(album), std::move(artist), std::move(title), std::move(path));
  }
};
} // namespace helpers

namespace model {
class Song : public Serializable {
  // This "model" can be owning or non-owning. Hence optional. Serializable::underlying_object always has pointer to the
  // actual protobuf message.
  std::optional<msw_proto_song::Song> proto_song_containing_optional_{};

public:
  Song();
  Song(msw_proto_song::Song&& song);
  Song(msw_proto_song::Song* song);
  Song(std::string album, std::string artist, std::string title, std::string fn);

  static Song deserialize(const std::string& contents);
  static Song deserialize_from_file(const std::string& file_on_disk);

  void set_artist(std::string&& artist);
  void set_album(std::string&& album);
  void set_title(std::string&& title);
  void set_path(std::string&& path);
  const std::string& album() const;
  const std::string& artist() const;

  friend bool operator==(const Song& lhs, const Song& rhs);

  friend bool operator!=(const Song& lhs, const Song& rhs);

  // operator msw_proto_song::Song&&();
  // void take_protobuf_song_out(std::function<void(msw_proto_song::Song*)> receiver);
  operator msw_proto_song::Song*() const;
  // operator msw_proto_song::Song();
};
} // namespace model
} // namespace msw

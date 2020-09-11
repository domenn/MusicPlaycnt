#pragma once

#include <optional>
// windows headers required to silence some warnings.
#include <src/win/windows_headers.hpp>

#include "../protobufs/songs.pb.h"
#include "serializable.hpp"
#include <ostream>

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
}  // namespace helpers

namespace model {
struct SongPartDifferences;
class Song;

struct SongPartDifferences {
  const unsigned int artist : 1;
  const unsigned int album : 1;
  const unsigned int title : 1;
  const unsigned int path : 1;

  const unsigned int equal_fields : 4;

  SongPartDifferences(unsigned artist, unsigned album, unsigned title, unsigned path)
      : artist(artist), album(album), title(title), path(path), equal_fields(artist + album + title + path) {}

  std::string write_disequality(msw::model::Song* left = nullptr, msw::model::Song* right = nullptr);

  friend std::ostream& operator<<(std::ostream& os, const SongPartDifferences& similarity);
  // friend std::ostream& operator<<(std::ostream& os, const SongSimilarityOstreamHelper& obj);

  struct SongSimilarityOstreamHelper {
    SongPartDifferences* ss{};
    msw::model::Song* left{nullptr};
    msw::model::Song* right{nullptr};

    friend std::ostream& operator<<(std::ostream& os, const SongSimilarityOstreamHelper& obj);
  };
};

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
  /**
   * Deprecated becouse there is better and more efficient way to do this. But I like this my implementation
   * nevertheless. So, leaving it in.
   */
  [[maybe_unused]] [[deprecated]] static void unused_ostream(std::ostream& os, const msw::model::Song& obj);
  static Song deserialize_from_file(const std::string& file_on_disk);

  void set_artist(std::string&& artist);
  void set_album(std::string&& album);
  void set_title(std::string&& title);
  void set_path(std::string&& path);
  const std::string& album() const;
  const std::string& artist() const;
  const std::string& title() const;
  const std::string& path() const;
  SongPartDifferences similarity(const Song& rhs) const;

  friend bool operator==(const Song& lhs, const Song& rhs);

  friend bool operator!=(const Song& lhs, const Song& rhs);

  operator msw_proto_song::Song*() const;

  friend std::ostream& operator<<(std::ostream& os, const Song& obj);
};
}  // namespace model
}  // namespace msw

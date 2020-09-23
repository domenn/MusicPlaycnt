#pragma once

#include <optional>
// windows headers required to silence some warnings.
#include <src/win/windows_headers.hpp>

#include "../protobufs/songs.pb.h"
#include "serializable.hpp"
#include <ostream>
#include <src/misc/utilities.hpp>

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
  struct SongSimilarityOstreamHelper {
    SongPartDifferences* ss{};
    msw::model::Song* left{nullptr};
    msw::model::Song* right{nullptr};

    friend std::ostream& operator<<(std::ostream& os, const SongSimilarityOstreamHelper& obj);
  };
};

std::ostream& operator<<(std::ostream& os, const SongPartDifferences::SongSimilarityOstreamHelper& obj);
std::ostream& operator<<(std::ostream& os, const SongPartDifferences& similarity);

class Song : public Serializable {
  static constexpr char TAGENCODE_KW_GENRE[] = "genre:";
  static constexpr size_t TAGENCODE_SZ_GENRE = sizeof TAGENCODE_KW_GENRE - 1;

  // This "model" can be owning or non-owning. Hence optional. Serializable::underlying_object always has pointer to the
  // actual protobuf message.
  std::optional<msw_proto_song::Song> proto_song_containing_optional_{};

  Song(std::string album,
       std::string artist,
       std::string title,
       std::string fn,
       int32_t plycnt,
       const google::protobuf::RepeatedPtrField<std::string>& tags);

 public:
  Song();
  Song(msw_proto_song::Song&& song);
  Song(msw_proto_song::Song* song);
  Song(std::string album, std::string artist, std::string title, std::string fn);

  Song(const Song& other) = delete;

  Song(Song&& other) noexcept;

  Song& operator=(const Song& other) = delete;

  Song& operator=(Song&& other) noexcept;
  void remove_tag_strict(std::string_view tag_to_remove);

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
  void add_tag(std::string&& tag);
  void set_playcount(int32_t val);
  void set_genre(std::string&& genre);
  std::string genre() const;
  const std::string& album() const;
  const std::string& artist() const;
  const std::string& title() const;
  const std::string& path() const;
  std::string delimited_tags(std::string_view delimiter = ",") const;
  int32_t playcount() const;
  SongPartDifferences similarity(const Song& rhs) const;

  bool operator==(const Song& rhs) const;

  bool operator!=(const Song& rhs) const;

  operator msw_proto_song::Song*() const;

  friend std::ostream& operator<<(std::ostream& os, const Song& obj);
  void increment_playcnt();

  Song make_copy() const;
  // void copy_into(msw_proto_song::Song* changing) const;

 private:  // utils - put to end for readability.
  auto internal_immutable_tags() const;
  template <typename proto_tags_t>
  static auto iterator_of_tags_starts_with_fining(proto_tags_t tags, const char* keyword, size_t kw_size);
  bool iterator_of_tags_valid(
      const google::protobuf::internal::RepeatedPtrIterator<const std::string>& repeated_ptr_iterator) const;
  std::string tags_extract(const google::protobuf::internal::RepeatedPtrIterator<const std::string>& cs,
                           size_t key_size) const;
  void remove_tag_by_iterator_if_valid(
      google::protobuf::internal::RepeatedPtrIterator<const std::string> temp_item) const;
  void remove_key_valued(const char* keyword, size_t kw_size) const;
  google::protobuf::internal::RepeatedPtrIterator<const std::string> iterator_of_tags_exact_match(
      std::string_view tag_to_remove) const;
};

std::ostream& operator<<(std::ostream& os, const Song& obj);

}  // namespace model
}  // namespace msw

inline bool msw::model::Song::iterator_of_tags_valid(
    const google::protobuf::internal::RepeatedPtrIterator<const std::string>& repeated_ptr_iterator) const {
  return repeated_ptr_iterator != reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->tags().end();
}

inline std::string msw::model::Song::tags_extract(
    const google::protobuf::internal::RepeatedPtrIterator<const std::string>& cs, size_t key_size) const {
  assert(iterator_of_tags_valid(cs));
  return cs->substr(key_size);
}

inline auto msw::model::Song::internal_immutable_tags() const {
  return reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->tags();
}

template <typename proto_tags_t>
auto msw::model::Song::iterator_of_tags_starts_with_fining(proto_tags_t tags, const char* keyword, size_t kw_size) {
  const auto temp_item = std::find_if(tags.begin(), tags.end(), [&](const std::string& t) {
    return msw::helpers::Utilities::starts_with(t, keyword, kw_size);
  });
  return temp_item;
}
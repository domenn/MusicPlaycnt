#include "song_list.hpp"

#include <google/protobuf/text_format.h>
#include <src/misc/custom_include_spdlog.hpp>

msw::model::SongList::SongList(msw_proto_song::Songs songs) : msw::Serializable(&songs_), songs_(std::move(songs)) {}

msw::model::SongList::SongList(SongList&& right) noexcept
    : msw::Serializable(&songs_), songs_(std::move(right.songs_)) {}

msw::model::SongList& msw::model::SongList::operator=(SongList&& other) noexcept {
  songs_ = std::move(other.songs_);
  return *this;
}

std::optional<msw::model::Song> msw::model::SongList::find_matching_song(const msw::model::Song& to_search) {
  const auto end_it = std::end(*songs_.mutable_songs());
  const auto beg_it = std::begin(*songs_.mutable_songs());
  auto found = std::find_if(
      beg_it, end_it, [&to_search](msw_proto_song::Song& protos) { return msw::model::Song(&protos) == to_search; });
  if (found != end_it) {
    return &*found;
  }
  return std::nullopt;
}

std::string msw::model::SongList::serialize_to_str() {
  return serialize(songs_);
}

void msw::model::SongList::make_song_and_add(std::string&& a, std::string&& b, std::string&& c, std::string&& d) {
  // TODO 3 inefficient impl. ; higher number is lower priority.
  templated_add_many(this, Song(a, b, c, d));
}

msw::model::Song msw::model::SongList::add_by_copying(const Song& song) {
  auto* new_proto_song = songs_.add_songs();
  // ReSharper disable once CppInitializedValueIsAlwaysRewritten
  Song new_song(new_proto_song);
  new_song = song.make_copy();
  return new_song;
}

// msw::model::SongListIterator msw::model::SongList::begin() { return std::begin(*songs_.mutable_songs()); }
//
// msw::model::SongListIterator msw::model::SongList::end() { return std::end(*songs_.mutable_songs()); }

msw::model::SongList::SongList(Song&& song) : Serializable(&songs_) {
  auto* new_song = songs_.add_songs();
  new_song->operator=(std::move(*song));
}

msw::model::SongList::SongList() : msw::Serializable(&songs_) {}

msw::model::Song msw::model::SongList::operator[](int idx) { return songs_.mutable_songs()->Mutable(idx); }

int msw::model::SongList::size() const { return songs_.songs_size(); }

// msw::model::SongList::SongList(msw_proto_song::Songs songs)
//  : songs_(std::move(songs)) {
//}

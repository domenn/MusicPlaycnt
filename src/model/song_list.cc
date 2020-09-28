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
  return similarity_find_impl(to_search, [](const msw::model::Song& l_to_search, msw_proto_song::Song& l_protos) {
    return msw::model::Song(&l_protos) == l_to_search;
  });
}

bool msw::model::SongList::wtf(const msw::model::Song& to_search, msw_proto_song::Song& protos) {
  const auto sim = msw::model::Song(&protos).similarity(to_search);
  return sim.artist && sim.title && sim.album && !sim.path;
}

std::optional<msw::model::Song> msw::model::SongList::similarity_find_impl(
    const msw::model::Song& to_search, std::function<bool(const msw::model::Song&, msw_proto_song::Song&)> searcher) {
  const auto found = similarity_find_impl_returns_iterator(to_search, searcher, std::begin(*songs_.mutable_songs()));
  if (found != std::end(*songs_.mutable_songs())) {
    return &*found;
  }
  return std::nullopt;
}

std::optional<msw::model::Song> msw::model::SongList::find_similar_was_song_moved_title_album_artist(
    const msw::model::Song& to_search) {
  return similarity_find_impl(to_search, [](const msw::model::Song& l_to_search, msw_proto_song::Song& l_protos) {
    const auto sim = msw::model::Song(&l_protos).similarity(l_to_search);
    return sim.artist && sim.title && sim.album && !sim.path;
  });
}

void msw::model::SongList::search_by_artist_title_or_all_four(
    const msw::model::Song& to_search, const std::function<bool(msw::model::Song&&)>& callback_single) {
  auto current_position = std::begin(*songs_.mutable_songs());
  auto start_search_here = current_position;
  while (true) {
    current_position = similarity_find_impl_returns_iterator(
        to_search,
        [](const msw::model::Song& l_to_search, msw_proto_song::Song& l_protos) {
          const auto sim = msw::model::Song(&l_protos).similarity(l_to_search);
          return sim.artist && sim.title;
        },
        start_search_here);
    if (current_position == std::end(*songs_.mutable_songs())) {
      break;
    }
    if (!callback_single(&*current_position)) {
      break;
    }
    start_search_here = current_position + 1;
  }
}

std::vector<msw::model::Song> msw::model::SongList::search_by_artist_title_or_all_four(
    const msw::model::Song& to_search) {
  std::vector<msw::model::Song> returning;
  search_by_artist_title_or_all_four(to_search, [&returning](msw::model::Song&& one_thingy) {
    returning.emplace_back(std::move(one_thingy));
    return true;
  });
  return returning;
}

std::string msw::model::SongList::serialize_to_str() { return serialize(songs_); }

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

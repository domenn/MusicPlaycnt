#include "song_list.hpp"

#include <google/protobuf/text_format.h>

#include <src/misc/custom_include_spdlog.hpp>

msw::model::SongList::SongList(msw_proto_song::Songs songs) : msw::Serializable(&songs_), songs_(std::move(songs)) {}

std::string msw::model::SongList::serialize() const {
  std::string returning;
  google::protobuf::io::StringOutputStream string_output_stream(&returning);
  google::protobuf::TextFormat::Print(songs_, &string_output_stream);
  return returning;
}

void msw::model::SongList::make_song_and_add(std::string&& a, std::string&& b, std::string&& c, std::string&& d) {
  // TODO 3 inefficient impl. ; higher number is lower priority.
  templated_add_many(this, Song(a, b, c, d));
}

msw::model::SongList::SongList(Song&& song) : Serializable(&songs_) {
  auto* new_song = songs_.add_songs();
  new_song->operator=(std::move(*song));
}

msw::model::Song msw::model::SongList::operator[](int idx) { return songs_.mutable_songs()->Mutable(idx); }

int msw::model::SongList::size() const { return songs_.songs_size(); }

// msw::model::SongList::SongList(msw_proto_song::Songs songs)
//  : songs_(std::move(songs)) {
//}

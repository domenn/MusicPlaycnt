#pragma once

#include "../protobufs/songs.pb.h"
#include "song.hpp"

namespace msw::model {
class SongList : public Serializable {
  msw_proto_song::Songs songs_{};

  void templated_add_many(SongList* s, Song&& first) { s->songs_.add_songs()->operator=(std::move(*first)); }

  template <typename... t_songs>
  void templated_add_many(SongList* s, Song&& first, t_songs&&... songs) {
    templated_add_many(s, std::move(first));
    templated_add_many(s, std::forward<t_songs>(songs)...);
  }

 public:
  std::string serialize() const;
  void make_song_and_add(std::string&& a, std::string&& b, std::string&& c, std::string&& d);

  SongList(msw_proto_song::Songs songs);
  SongList(Song&& song);

  template <typename... t_songs>
  SongList(Song&& first, t_songs&&... songs) : SongList(std::move(first)) {
    templated_add_many(this, std::forward<t_songs>(songs)...);
  }

  void add(Song&& one) { templated_add_many(this, std::move(one)); }

  [[nodiscard]] Song operator[](int idx);
  [[nodiscard]] int size() const;
};
}  // namespace msw::model

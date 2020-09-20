#pragma once

#include "../protobufs/songs.pb.h"
#include "song.hpp"

namespace msw::model {
//
//class SongListIterator {
//  // iterator traits
//  // using difference_type = int;
//  using value_type = const msw::model::Song;
//  // using pointer = const std::string*;
//  // using reference = const std::string&;
//  using iterator_category = std::forward_iterator_tag;
//
//  google::protobuf::RepeatedPtrField<msw_proto_song::Song>::iterator proto_song_iterator_;
//  msw::model::Song temp_song_;
//  bool valid_{false};
//
// public:
//  SongListIterator(google::protobuf::RepeatedPtrField<msw_proto_song::Song>::iterator proto_song_iterator)
//      : proto_song_iterator_(std::move(proto_song_iterator)) {}
//
//  // operator msw::model::Song() const { return msw::model::Song(&(*proto_song_iterator_)); }
//
//  SongListIterator& operator++() {
//    valid_ = false;
//    ++proto_song_iterator_;
//    return *this;
//  }
//
//  SongListIterator operator++(int) {
//    valid_ = false;
//    SongListIterator retval = *this;
//    ++(*this);
//    return retval;
//  }
//
//  bool operator==(const SongListIterator& x) const { return proto_song_iterator_ == x.proto_song_iterator_; }
//  bool operator!=(const SongListIterator& x) const { return proto_song_iterator_ != x.proto_song_iterator_; }
//  void validate() {
//    if (!valid_) {
//      temp_song_ = &(*proto_song_iterator_);
//      valid_ = true;
//    }
//  }
//
//  msw::model::Song* operator*() {
//    validate();
//    return &temp_song_;
//  }
//  msw::model::Song& operator->() {
//    validate();
//    return temp_song_;
//  }
//};

class SongList : public Serializable {
  msw_proto_song::Songs songs_{};

  void templated_add_many(SongList* s, Song&& first) { s->songs_.add_songs()->operator=(std::move(*first)); }

  template <typename... t_songs>
  void templated_add_many(SongList* s, Song&& first, t_songs&&... songs) {
    templated_add_many(s, std::move(first));
    templated_add_many(s, std::forward<t_songs>(songs)...);
  }

 public:
  std::string serialize_to_str();
  void make_song_and_add(std::string&& a, std::string&& b, std::string&& c, std::string&& d);
  Song add_by_copying(const Song& song);
  // SongListIterator begin();
  // SongListIterator end();

  SongList(msw_proto_song::Songs songs);
  SongList(SongList&& right) noexcept;
  SongList& operator=(SongList&& other) noexcept;
  SongList(Song&& song);
  SongList(const SongList& other) = delete;
  SongList& operator=(const SongList& other) = delete;
  SongList();

  template <typename... t_songs>
  SongList(Song&& first, t_songs&&... songs) : SongList(std::move(first)) {
    templated_add_many(this, std::forward<t_songs>(songs)...);
  }

  void add(Song&& one) { templated_add_many(this, std::move(one)); }

  [[nodiscard]] Song operator[](int idx);
  [[nodiscard]] int size() const;
  [[nodiscard]] std::optional<Song> find_matching_song(const msw::model::Song& to_search);
};
}  // namespace msw::model

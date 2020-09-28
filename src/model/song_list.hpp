#pragma once

#include "../protobufs/songs.pb.h"
#include "song.hpp"

namespace msw::model {
//
// class SongListIterator {
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

  template <typename t_it>
  [[nodiscard]] auto similarity_find_impl_returns_iterator(
      const msw::model::Song& to_search,
      std::function<bool(const msw::model::Song&, msw_proto_song::Song&)> searcher,
      t_it beg_it) {
    return std::find_if(beg_it,
                        std::end(*songs_.mutable_songs()),
                        [&to_search, &searcher](msw_proto_song::Song& protos) { return searcher(to_search, protos); });
  }
  [[nodiscard]] std::optional<Song> similarity_find_impl(
      const msw::model::Song& to_search, std::function<bool(const msw::model::Song&, msw_proto_song::Song&)> searcher);

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
  static bool wtf(const msw::model::Song& to_search, msw_proto_song::Song& protos);
  [[nodiscard]] std::optional<Song> find_similar_was_song_moved_title_album_artist(const msw::model::Song& to_search);
  /**
   * Run function (`callback_single`) for each song that matches by artist or title or both or all.)
   *
   * \param to_search to try match (to test with).
   * \param callback_single To be applied on results, one at a time. Returning false will stop the loop, returning true
   * will continue the loop / the search until it visits all the songs.
   */
  void search_by_artist_title_or_all_four(const msw::model::Song& to_search,
                                          const std::function<bool(msw::model::Song&&)>& callback_single);
  std::vector<msw::model::Song> search_by_artist_title_or_all_four(const msw::model::Song& to_search);
};
}  // namespace msw::model

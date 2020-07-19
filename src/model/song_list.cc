#include "song_list.hpp"
#include <src/misc/custom_include_spdlog.hpp>

#include <google/protobuf/text_format.h>

void msw::model::SongList::example_songlists() {

  msw_proto_song::Song proto_song;
  proto_song.set_album("oneAlbum1");
  proto_song.set_artist("oneArtist1");
  proto_song.set_title("TITTELLL!!!! 11");
  proto_song.set_path("OnePath .... 1");
  proto_song.set_playcnt(421);

  msw_proto_song::Song proto_song1;
  proto_song1.set_album("oneAlbum2");
  proto_song1.set_artist("oneArtist2");
  proto_song1.set_title("22TITTELLL!!!! 11");
  proto_song1.set_path("OnePath .... 2");
  proto_song1.set_playcnt(1421);

  msw_proto_song::Songs my_songs;
  my_songs.mutable_songs()->Add(std::move(proto_song));
  my_songs.mutable_songs()->Add(std::move(proto_song1));

  SongList songlist(std::move(my_songs));
  std::string serd = songlist.serialize();
  SPDLOG_INFO(serd);
}
std::string msw::model::SongList::serialize() const {
  std::string returning;
  google::protobuf::io::StringOutputStream string_output_stream(&returning);
  google::protobuf::TextFormat::Print(songs_, &string_output_stream);
  return returning;
}
msw::model::SongList::SongList(msw_proto_song::Songs  songs) : songs_(std::move(songs)) {}

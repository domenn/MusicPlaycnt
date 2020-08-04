#include "song.hpp"

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include <src/misc/custom_include_spdlog.hpp>
#include <sstream>
#include <utility>

#include "../misc/open_wrap.hpp"
#include "serializable.hpp"
#include "src/win/winapi_exceptions.hpp"

msw::model::Song msw::model::Song::deserialize_from_file(const std::string& file_on_disk) {
  msw_proto_song::Song empty_proto_song;
  auto [handle, my_errno] = cp_open_lw(file_on_disk.c_str(), O_RDONLY);
  if (handle == -1) {
    SPDLOG_CRITICAL("Couldn't get handle. err number is {}", my_errno);
    throw msw::exceptions::ErrorCode(my_errno,
                                     M_PROTOBUF_OPEN_FILE_IMPL,
                                     MSW_TRACE_ENTRY_CREATE,
                                     ("Unable to open " + file_on_disk + " as readonly.").c_str());
  }
  google::protobuf::io::FileInputStream proto_is(handle);
  //#ifndef _MSC_VER
  proto_is.SetCloseOnDelete(true);
  //#endif
  google::protobuf::TextFormat::Parse(&proto_is, &empty_proto_song);
  //   proto_is.Close();
  return empty_proto_song;
}

msw::model::Song::Song(msw_proto_song::Song song)
  : proto_song_containing_optional_(std::move(song)) {
  underlying_object_ = &proto_song_containing_optional_.value();
}

msw::model::Song::Song(msw_proto_song::Song* song)
  : Serializable(song) {
}

void msw::model::Song::testing_create_restore() {
  msw_proto_song::Song proto_song;
  proto_song.set_album("oneAlbum");
  proto_song.set_artist("oneArtist");
  proto_song.set_title("TITTELLL!!!!");
  proto_song.set_path("OnePath ....");
  proto_song.set_playcnt(42);

  Song song1(std::move(proto_song));
  std::string serd = song1.serialize();
  song1.serialize(std::string("C:\\users\\public\\f1.txt"));
  SPDLOG_INFO("Serialized first " + serd);

  Song song2 = deserialize(serd);
  SPDLOG_INFO("Restored and again serd ... what happens: {}", serd);
}

void msw::model::Song::set_artist(std::string&& artist) {
  reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->set_artist(std::move(artist));
}

void msw::model::Song::set_album(std::string&& album) {
  reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->set_album(std::move(album));
}

void msw::model::Song::set_title(std::string&& title) {
  reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->set_title(std::move(title));
}

void msw::model::Song::set_path(std::string&& path) {
  reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->set_path(std::move(path));
}

const std::string& msw::model::Song::album() const {
  return reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->album();
}

msw::model::Song msw::model::Song::deserialize(const std::string& contents) {
  msw_proto_song::Song empty_proto_song;

  std::istringstream istringstr(contents);
  google::protobuf::io::IstreamInputStream proto_iss(&istringstr);
  google::protobuf::TextFormat::Parse(&proto_iss, &empty_proto_song);
  return empty_proto_song;
}

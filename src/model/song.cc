#include "song.hpp"

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include <src/misc/custom_include_spdlog.hpp>
#include <sstream>
#include <utility>

#include "../misc/open_wrap.hpp"
#include "serializable.hpp"

msw::model::Song msw::model::Song::deserialize_from_file(const std::string& file_on_disk) {
  msw_proto_song::Song empty_proto_song;
  auto [handle, my_errno] = cp_open_lw(file_on_disk.c_str(), O_RDONLY);
  if (handle == -1) {
    // throw ErrCodeException(fmt::format("At: {}{}{}{}\n Cannot read file: {}. open() error (code {{}} - {}): {}",
    //                                   D_U_M_FUNC_FILE_LINE_TRACE_IN_FMT,
    //                                   str_config_file_path,
    //                                   errnoname(my_errno),
    //                                   d_common::safe::strerror(my_errno)),
    //                       my_errno);
    SPDLOG_CRITICAL("Couldn't get handle. err number is {}", my_errno);
    throw std::runtime_error("cp_open_lw (protobuf low level file IO) failed with code " + std::to_string(my_errno));
  }
  google::protobuf::io::FileInputStream proto_is(handle);
  //#ifndef _MSC_VER
  proto_is.SetCloseOnDelete(true);
  //#endif
  google::protobuf::TextFormat::Parse(&proto_is, &empty_proto_song);
  //   proto_is.Close();
  return empty_proto_song;
}

msw::model::Song::Song(msw_proto_song::Song song) : Serializable(proto_song_), proto_song_(std::move(song)) {}

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

msw::model::Song msw::model::Song::deserialize(const std::string& contents) {
  msw_proto_song::Song empty_proto_song;

  std::istringstream istringstr(contents);
  google::protobuf::io::IstreamInputStream proto_iss(&istringstr);
  google::protobuf::TextFormat::Parse(&proto_iss, &empty_proto_song);
  return empty_proto_song;
}
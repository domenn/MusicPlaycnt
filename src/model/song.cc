#include "song.hpp"

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include <src/misc/custom_include_spdlog.hpp>
#include <sstream>
#include <utility>

#include "../misc/open_wrap.hpp"
#include "serializable.hpp"
#include "src/win/winapi_exceptions.hpp"
#include <google/protobuf/util/message_differencer.h>

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

msw::model::Song::Song()
  : proto_song_containing_optional_{std::in_place} {
  underlying_object_ = &proto_song_containing_optional_.value();
}

msw::model::Song::Song(msw_proto_song::Song&& song)
  : proto_song_containing_optional_(std::move(song)) {
  underlying_object_ = &proto_song_containing_optional_.value();
}

msw::model::Song::Song(msw_proto_song::Song* song)
  : Serializable(song) {
}

msw::model::Song::Song(std::string album, std::string artist, std::string title, std::string fn)
  : model::Song() {
  set_album(std::move(album));
  set_artist(std::move(artist));
  set_title(std::move(title));
  set_path(std::move(fn));
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

const std::string& msw::model::Song::artist() const {
  return reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->artist();
}

const std::string& msw::model::Song::album() const {
  return reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->album();
}


//msw::model::Song::operator msw_proto_song::Song&&() {
//  if (proto_song_containing_optional_) {
//    return std::move(proto_song_containing_optional_.value());
//  }
//  return reinterpret_cast<msw_proto_song::Song&&>(*underlying_object_);
//}

//void msw::model::Song::take_protobuf_song_out(std::function<void(msw_proto_song::Song*)> receiver) {
//  receiver(reinterpret_cast<msw_proto_song::Song*>(underlying_object_));
//  if (proto_song_containing_optional_) {
//    proto_song_containing_optional_.reset();
//  }
//}

msw::model::Song::operator msw_proto_song::Song*() const {
  return reinterpret_cast<msw_proto_song::Song*>(underlying_object_);
}

msw::model::Song msw::model::Song::deserialize(const std::string& contents) {
  return from_string<msw_proto_song::Song>(contents);
}

bool msw::model::operator==(const Song& lhs, const Song& rhs) {
  return google::protobuf::util::MessageDifferencer::Equals(*lhs.underlying_object_, *rhs.underlying_object_);
}

bool msw::model::operator!=(const Song& lhs, const Song& rhs) {
  return !(lhs == rhs);
}

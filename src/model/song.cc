#include "song.hpp"

#include "../misc/open_wrap.hpp"
#include "serializable.hpp"
#include "src/win/winapi_exceptions.hpp"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/message_differencer.h>
#include <src/data/pointers_to_globals.hpp>
#include <src/misc/consts.hpp>
#include <src/misc/custom_include_spdlog.hpp>
#include <src/model/app_config.hpp>
#include <utility>

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

std::string msw::model::SongPartDifferences::write_disequality(msw::model::Song* left, msw::model::Song* right) {
  std::ostringstream stostr;
  stostr << msw::model::SongPartDifferences::SongSimilarityOstreamHelper{this, left, right};
  return stostr.str();
}

msw::model::Song::Song() : proto_song_containing_optional_{std::in_place} {
  underlying_object_ = &proto_song_containing_optional_.value();
}

msw::model::Song::Song(msw_proto_song::Song&& song) : proto_song_containing_optional_(std::move(song)) {
  underlying_object_ = &proto_song_containing_optional_.value();
}

msw::model::Song::Song(msw_proto_song::Song* song) : Serializable(song) {}

msw::model::Song::Song(std::string album, std::string artist, std::string title, std::string fn) : model::Song() {
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

const std::string& msw::model::Song::title() const {
  return reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->title();
}

const std::string& msw::model::Song::path() const {
  return reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->path();
}

msw::model::SongPartDifferences msw::model::Song::similarity(const Song& rhs) const {
  return SongPartDifferences{
      artist() == rhs.artist(), album() == rhs.album(), title() == rhs.title(), path() == rhs.path()};
}

const std::string& msw::model::Song::album() const {
  return reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->album();
}

msw::model::Song::operator msw_proto_song::Song*() const {
  return reinterpret_cast<msw_proto_song::Song*>(underlying_object_);
}

msw::model::Song msw::model::Song::deserialize(const std::string& contents) {
  return from_string<msw_proto_song::Song>(contents);
}

void msw::model::Song::increment_playcnt() {
  auto* casted = reinterpret_cast<msw_proto_song::Song*>(underlying_object_);
  casted->set_playcnt(casted->playcnt() + 1);
}

bool msw::model::operator==(const Song& lhs, const Song& rhs) {
  return google::protobuf::util::MessageDifferencer::Equals(*lhs.underlying_object_, *rhs.underlying_object_);
}

bool msw::model::operator!=(const Song& lhs, const Song& rhs) { return !(lhs == rhs); }

void replace_one(std::string& mutated, const char* const finding, size_t finding_size, const std::string& new_thing) {
  if (auto idx = mutated.find(finding, 0, finding_size); idx != std::string::npos) {
    mutated.replace(idx, finding_size, new_thing);
  }
}

#define MACRO_STR_AND_SZ(macro_item) macro_item, sizeof macro_item - 1

void msw::model::Song::unused_ostream(std::ostream& os, const Song& obj) {
  const auto& me = *reinterpret_cast<msw_proto_song::Song*>(obj.underlying_object_);
  std::string my_result = msw::pg::app_config->song_print_format();

  replace_one(my_result, MACRO_STR_AND_SZ(consts::KW_TOKEN_ARTIST), obj.artist());
  replace_one(my_result, MACRO_STR_AND_SZ(consts::KW_TOKEN_AL), obj.album());
  replace_one(my_result, MACRO_STR_AND_SZ(consts::KW_TOKEN_P), obj.path());
  replace_one(my_result, MACRO_STR_AND_SZ(consts::KW_TOKEN_T), obj.title());
}

std::ostream& msw::model::operator<<(std::ostream& os, const Song& obj) {
  return os << fmt::format(msw::pg::app_config->song_print_format(),
                           fmt::arg(consts::KW_TOKEN_ARTIST, obj.artist()),
                           fmt::arg(consts::KW_TOKEN_AL, obj.album()),
                           fmt::arg(consts::KW_TOKEN_P, obj.path()),
                           fmt::arg(consts::KW_TOKEN_T, obj.title()));
}

std::ostream& msw::model::operator<<(std::ostream& os,
                                     const msw::model::SongPartDifferences::SongSimilarityOstreamHelper& obj) {
  if (obj.left && obj.right) {
    os << "Songs\n" << *obj.left << "\nAND\n" << *obj.right << "\ndiffer IN:";
  } else if (obj.left || obj.right) {
    os << "Song\n" << (obj.left ? *obj.left : *obj.right) << "\ndiffers IN:";
  }
  os << *obj.ss;
  return os;
}
std::ostream& msw::model::operator<<(std::ostream& os, const msw::model::SongPartDifferences& similarity) {
  if (!similarity.artist) {
    os << " artist\n";
  }
  if (!similarity.album) {
    os << " album\n";
  }
  if (!similarity.title) {
    os << " title\n";
  }
  if (!similarity.path) {
    os << " path\n";
  }
  return os;
}

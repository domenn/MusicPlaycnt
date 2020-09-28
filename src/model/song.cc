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
#include <src/misc/utilities.hpp>
#include <src/model/app_config.hpp>
#include <utility>

#define INTERNAL_DECLARE_IMMUTABLE_TAGS \
  const auto tags = reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->tags()
#define INTERNAL_DECLARE_MUTABLE_TAGS \
  auto* tags = reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->mutable_tags()

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

msw::model::Song::Song(std::string album,
                       std::string artist,
                       std::string title,
                       std::string fn,
                       int32_t plycnt,
                       const google::protobuf::RepeatedPtrField<std::string>& tags)
    : msw::model::Song(album, artist, title, fn) {
  auto* s = reinterpret_cast<msw_proto_song::Song*>(underlying_object_);
  s->set_playcnt(plycnt);
  s->mutable_tags()->CopyFrom(tags);
}

msw::model::Song::Song(Song&& other) noexcept
    : proto_song_containing_optional_(std::move(other.proto_song_containing_optional_)) {
  if (proto_song_containing_optional_.has_value()) {
    underlying_object_ = &proto_song_containing_optional_.value();
  } else {
    underlying_object_ = other.underlying_object_;
  }
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

void msw::model::Song::add_tag(std::string&& tag) {
  reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->add_tags(std::move(tag));
}

void msw::model::Song::set_playcount(int32_t val) {
  reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->set_playcnt(val);
}

void msw::model::Song::internal_remove_genre(const char* keyword, size_t kw_size) const {
  INTERNAL_DECLARE_MUTABLE_TAGS;
  const auto iterator = std::find_if(tags->begin(), tags->end(), tags_kw_find_if_genre);
  if (iterator != tags->end()) {
    tags->erase(iterator);
  }
}
// // TODO d tbd
// void msw::model::Song::remove_tag_by_iterator_if_valid(
//    const google::protobuf::internal::RepeatedPtrIterator<const std::string> temp_item) const {
//  if (iterator_of_tags_valid(temp_item)) {
//    SPDLOG_TRACE("It valid: {}", *temp_item);
//    reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->mutable_tags()->erase(temp_item);
//  }
//}

void msw::model::Song::set_genre(std::string&& genre) {
  SPDLOG_TRACE("Setting g {}", genre);
  internal_remove_genre(TAGENCODE_KW_GENRE, TAGENCODE_SZ_GENRE);
  if (!genre.empty()) {
    add_tag(TAGENCODE_KW_GENRE + std::move(genre));
    // SPDLOG_TRACE("set; After is {}", this->genre());
  }
}

std::string msw::model::Song::genre() const {
  INTERNAL_DECLARE_IMMUTABLE_TAGS;
  const auto temp_item = std::find_if(std::begin(tags), std::end(tags), tags_kw_find_if_genre);
  if (temp_item != std::end(tags)) {
    SPDLOG_TRACE("Iterator is valid and it is {}", *temp_item);
    return temp_item->substr(TAGENCODE_SZ_GENRE);
  }
  return {};
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

std::string msw::model::Song::delimited_tags(std::string_view delimiter) const {
  INTERNAL_DECLARE_IMMUTABLE_TAGS;
  std::string returning{};
  for (const auto& tag : tags) {
    returning.append(tag).append(delimiter);
  }
  if (!returning.empty()) {
    returning.erase(returning.end() - delimiter.size(), returning.end());
  }
  return returning;
}

int32_t msw::model::Song::playcount() const {
  return reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->playcnt();
}

msw::model::SongPartDifferences msw::model::Song::similarity(const Song& rhs) const {
  return SongPartDifferences{
      artist() == rhs.artist(), album() == rhs.album(), title() == rhs.title(), path() == rhs.path()};
}

bool msw::model::Song::operator==(const Song& rhs) const {
  google::protobuf::util::MessageDifferencer dif;
  dif.IgnoreField(msw_proto_song::Song::GetDescriptor()->FindFieldByNumber(msw_proto_song::Song::kPlaycntFieldNumber));
  dif.IgnoreField(msw_proto_song::Song::GetDescriptor()->FindFieldByNumber(msw_proto_song::Song::kTagsFieldNumber));
  return dif.Compare(*underlying_object_, *rhs.underlying_object_);
}

bool msw::model::Song::operator!=(const Song& rhs) const { return !operator==(rhs); }

const std::string& msw::model::Song::album() const {
  return reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->album();
}

msw::model::Song::operator msw_proto_song::Song *() const {
  return reinterpret_cast<msw_proto_song::Song*>(underlying_object_);
}

msw::model::Song& msw::model::Song::operator=(Song&& other) noexcept {
  if (this == &other) return *this;
  // if(other.proto_song_containing_optional_ != std::nullopt) {
  //  proto_song_containing_optional_ = std::move(other.proto_song_containing_optional_);
  //}else {
  auto other_song = *reinterpret_cast<msw_proto_song::Song*>(other.underlying_object_);
  reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->operator=(std::move(other_song));
  return *this;
}

void msw::model::Song::remove_tag_strict(std::string_view tag_to_remove) {
  INTERNAL_DECLARE_MUTABLE_TAGS;
  const auto found_it = std::find(tags->begin(), tags->end(), tag_to_remove);
  if (found_it != tags->end()) {
    tags->erase(found_it);
  }
}

msw::model::Song msw::model::Song::deserialize(const std::string& contents) {
  return from_string<msw_proto_song::Song>(contents);
}

void msw::model::Song::increment_playcnt() {
  auto* casted = reinterpret_cast<msw_proto_song::Song*>(underlying_object_);
  casted->set_playcnt(casted->playcnt() + 1);
}

msw::model::Song msw::model::Song::make_copy() const {
  return Song(album(),
              artist(),
              title(),
              path(),
              playcount(),
              reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->tags());
}

// void msw::model::Song::copy_into(msw_proto_song::Song* changing) const {
//
//}

// bool msw::model::operator==(const Song& lhs, const Song& rhs) {
//  return google::protobuf::util::MessageDifferencer::Equals(*lhs.underlying_object_, *rhs.underlying_object_);
//}
//
// bool msw::model::operator!=(const Song& lhs, const Song& rhs) { return !(lhs == rhs); }

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

// // // TODO d tbd
// void msw::model::Song::remove_tag_by_iterator_if_valid(
//    const google::protobuf::internal::RepeatedPtrIterator<const std::string> temp_item) const {
//  SPDLOG_TRACE("We're asked to remove tag by iterator ...");
//  if (iterator_of_tags_valid(temp_item)) {
//    SPDLOG_TRACE("It valid: {}", *temp_item);
//    reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->mutable_tags()->erase(temp_item);
//  }
//}
//
// google::protobuf::internal::RepeatedPtrIterator<const std::string> msw::model::Song::iterator_of_tags_exact_match(
//    std::string_view tag_to_remove) const {
//  const auto& tags = reinterpret_cast<msw_proto_song::Song*>(underlying_object_)->tags();
//  return std::find(tags.begin(), tags.end(), tag_to_remove);
//}

#include "song_with_metadata.hpp"

#include <src/misc/utiltime.hpp>

msw::model::SongWithMetadata::SongWithMetadata(SongWithMetadata&& other) noexcept
    : Serializable(&proto_s_), proto_s_(std::move(other.proto_s_)) {}

msw::model::SongWithMetadata& msw::model::SongWithMetadata::operator=(SongWithMetadata&& other) noexcept {
  proto_s_ = std::move(other.proto_s_);
  return *this;
}

msw::model::SongWithMetadata::SongWithMetadata(msw::model::Song&& song,
                                               ActionType action_type,
                                               uint64_t action_timestamp,
                                               uint64_t playing_ms)
    : Serializable(&proto_s_) {
  auto* stolen_song = static_cast<msw_proto_song::Song*>(song);
  proto_s_.mutable_song()->operator=(std::move(*stolen_song));
  set_timestamp_of_action(action_timestamp);
  set_action_type(action_type);
  set_playing_ms(playing_ms);
}

void msw::model::SongWithMetadata::set_timestamp_of_action(uint64_t ts) { proto_s_.set_action_timestamp(ts); }

void msw::model::SongWithMetadata::set_action_type(ActionType ty) {
  proto_s_.set_action_type(static_cast<msw_proto_song::SongWithMetadata_ActionType>(ty));
}

msw::model::Song msw::model::SongWithMetadata::get_song() { return proto_s_.mutable_song(); }

// msw::model::Song msw::model::SongWithMetadata::get_song() const {
//  auto ms = proto_s_.song();
//  return msw::model::Song(std::addressof(ms));
//}

std::string msw::model::SongWithMetadata::serialize_to_str() { return serialize(proto_s_); }

void msw::model::SongWithMetadata::on_stop_or_pause(ActionType new_action_type, uint64_t action_timestamp) {
  assert(new_action_type == ActionType::PAUSE || new_action_type == ActionType::STOP);
  if (action_type() == ActionType::PLAY) {
    set_playing_ms(recalculate_playing_time(action_timestamp));
  }
  set_action_type(new_action_type);
  set_timestamp_of_action(action_timestamp);
}

uint64_t msw::model::SongWithMetadata::action_timestamp() const { return proto_s_.action_timestamp(); }

uint64_t msw::model::SongWithMetadata::playing_ms() const { return proto_s_.playing_ms(); }

void msw::model::SongWithMetadata::set_playing_ms(uint64_t value) { proto_s_.set_playing_ms(value); }

bool msw::model::SongWithMetadata::invalid() const {
  return proto_s_.song().artist().empty() || proto_s_.song().title().empty();
}

msw::model::ActionType msw::model::SongWithMetadata::action_type() {
  return static_cast<msw::model::ActionType>(proto_s_.action_type());
}

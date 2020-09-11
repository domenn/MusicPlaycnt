#include "song_with_metadata.hpp"

msw::model::SongWithMetadata::SongWithMetadata(SongWithMetadata&& other) noexcept
    : Serializable(&proto_s_), proto_s_(std::move(other.proto_s_)) {}

msw::model::SongWithMetadata& msw::model::SongWithMetadata::operator=(SongWithMetadata&& other) noexcept {
  proto_s_ = std::move(other.proto_s_);
  return *this;
}

void msw::model::SongWithMetadata::set_timestamp_of_action(uint64_t ts) { proto_s_.set_action_timestamp(ts); }

void msw::model::SongWithMetadata::set_action_type(ActionType ty) {
  proto_s_.set_action_type(static_cast<msw_proto_song::SongWithMetadata_ActionType>(ty));
}

msw::model::Song msw::model::SongWithMetadata::get_song() { return proto_s_.mutable_song(); }

std::string msw::model::SongWithMetadata::serialize_to_str() { return serialize(proto_s_); }
uint64_t msw::model::SongWithMetadata::action_timestamp() const { return proto_s_.action_timestamp(); }

uint64_t msw::model::SongWithMetadata::playing_seconds() const {
  return proto_s_.playing_seconds();
}

void msw::model::SongWithMetadata::set_playing_seconds(uint64_t seconds) {
  proto_s_.set_playing_seconds(seconds);
}

msw::model::ActionType msw::model::SongWithMetadata::action_type() {
  return static_cast<msw::model::ActionType>(proto_s_.action_type());
}

#include "song_with_metadata.hpp"

void msw::model::SongWithMetadata::set_timestamp_of_action(uint64_t ts) {
  proto_s_.set_action_timestamp(ts);
}

void msw::model::SongWithMetadata::set_action_type(ActionType ty) {
  proto_s_.set_action_type(static_cast<msw_proto_song::SongWithMetadata_ActionType>(ty));
}

msw::model::Song msw::model::SongWithMetadata::get_song() {
  return proto_s_.mutable_song();
}

std::string msw::model::SongWithMetadata::serialize_to_str() {
  return serialize(proto_s_);
}

#pragma once
#include "action_type.hpp"
#include "serializable.hpp"
#include "song.hpp"
#include "src/protobufs/songs.pb.h"

namespace msw::model {
class SongWithMetadata : public Serializable {
  msw_proto_song::SongWithMetadata proto_s_{};

 public:
  SongWithMetadata() : Serializable(&proto_s_) {}

  SongWithMetadata(const SongWithMetadata& other) = delete;
  SongWithMetadata(SongWithMetadata&& other) noexcept;
  SongWithMetadata& operator=(const SongWithMetadata& other) = delete;
  SongWithMetadata& operator=(SongWithMetadata&& other) noexcept;
  ~SongWithMetadata() = default;

  void set_timestamp_of_action(uint64_t);
  void set_action_type(ActionType);

  msw::model::Song get_song();
  ActionType action_type();
  uint64_t timestamp();

  std::string serialize_to_str();
};
}  // namespace msw::model

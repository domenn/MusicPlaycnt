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
  explicit SongWithMetadata(msw::model::Song&& song,
                            ActionType action_type = ActionType::PLAY,
                            uint64_t action_timestamp = 0,
                            uint64_t playing_ms = 0);

  void set_timestamp_of_action(uint64_t);
  void set_action_type(ActionType);

  msw::model::Song get_song();
  ActionType action_type();
  uint64_t action_timestamp() const;
  uint64_t playing_ms() const;
  void set_playing_ms(uint64_t value);
  std::string serialize_to_str();
  /**
   * Sets the new state. And also updates own playtime.
   *
   * \param new_action_type New action type to set.
   */
  void on_stop_or_pause(ActionType new_action_type, uint64_t action_timestamp);

  uint64_t recalculate_playing_time(uint64_t action_timestamp) const ;
};
}  // namespace msw::model

inline uint64_t msw::model::SongWithMetadata::recalculate_playing_time(const uint64_t action_timestamp) const {
    return action_timestamp - this->action_timestamp() + playing_ms();
}
#pragma once
#include "serializable.hpp"
#include "src/protobufs/songs.pb.h"
#include "action_type.hpp"
#include "song.hpp"

namespace msw::model {
class SongWithMetadata : public Serializable {

  msw_proto_song::SongWithMetadata proto_s_{};

public:
  SongWithMetadata()
    : Serializable(&proto_s_) {
  }

  void set_timestamp_of_action(uint64_t);
  void set_action_type(ActionType);

  msw::model::Song get_song();
  std::string serialize_to_str();
};
}

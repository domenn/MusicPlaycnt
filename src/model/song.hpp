#pragma once
#include "../protobufs/songs.pb.h"

namespace msw::model {
class Song {

  msw_proto_song::Song song_;

 public:
  Song(msw_proto_song::Song  song);

 public:
  std::string serialize() const;
  static Song deserialize_from_file(const std::string& file_on_disk);
  static Song deserialize(const std::string& file_on_disk);

  static void testing_create_restore();
};
}  // namespace msw::model
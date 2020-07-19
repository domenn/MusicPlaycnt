#pragma once

#include <src/win/windows_headers.hpp>
#include "../protobufs/songs.pb.h"

namespace msw::model {
class SongList {
  msw_proto_song::Songs songs_;
  SongList(msw_proto_song::Songs  songs);


 public:


 public:
  std::string serialize() const;

  static void example_songlists();

};
}  // namespace msw::model
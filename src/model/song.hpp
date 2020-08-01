#pragma once

#include <src/win/windows_headers.hpp>

#include "../protobufs/songs.pb.h"
#include "serializable.hpp"

namespace msw::model {
class Song : public Serializable {
  msw_proto_song::Song proto_song_{};
public:
  Song(msw_proto_song::Song song);

public:
  static Song deserialize(const std::string& contents);
  static Song deserialize_from_file(const std::string& file_on_disk);

  static void testing_create_restore();
};
} // namespace msw::model

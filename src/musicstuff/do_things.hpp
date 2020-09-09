#pragma once

#include <src/model/song_with_metadata.hpp>
namespace msw {
namespace do_things {
void new_song_happened(msw::model::SongWithMetadata&& new_song);
}
}  // namespace msw
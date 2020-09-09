#pragma once

namespace msw {

namespace model {
class Song;
class SongWithMetadata;
class SongList;
} // namespace model

namespace data {
struct DataForTests;

template <typename T>
class Accessor;
} // namespace data

namespace pg {
extern data::Accessor<msw::model::SongList>* song_list;
extern data::Accessor<msw::model::SongWithMetadata>* handled_song;

// optional, only used in tests. Should cause linker error if used in non-test.
extern data::DataForTests* data_for_tests;
} // namespace pg

} // namespace msw

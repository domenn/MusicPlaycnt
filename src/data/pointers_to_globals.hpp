#pragma once

namespace msw {

namespace model {
class Song;
class SongWithMetadata;
class SongList;
}  // namespace model

namespace data {

template <typename T>
class Accessor;

template class Accessor<msw::model::SongList>;
}  // namespace data

namespace pg {
extern data::Accessor<msw::model::SongList>* song_list;
extern data::Accessor<msw::model::SongWithMetadata>* handled_song;
}  // namespace pg

}  // namespace msw

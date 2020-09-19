#include "production_accessor.hpp"

#include <src/model/app_config.hpp>
#include <src/model/song_with_metadata.hpp>

template <typename T>
std::unique_ptr<T> msw::data::ProductionAccessor<T>::initial_load_function() {
  return std::make_unique<T>(Serializable::from_file<T>(path_for_disk_storage_));
}

template <typename T>
void msw::data::ProductionAccessor<T>::persist(T* item) {
  item->serialize(path_for_disk_storage_);
}

template <typename T>
msw::data::ProductionAccessor<T>::ProductionAccessor(const std::string& path_for_disk_storage)
    : path_for_disk_storage_(path_for_disk_storage) {}

// template class msw::data::ProductionAccessor<msw::model::SongList>;
template class msw::data::ProductionAccessor<msw::model::SongWithMetadata>;

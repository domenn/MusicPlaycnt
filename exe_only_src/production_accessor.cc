#include "production_accessor.hpp"

#include <src/model/song_list.hpp>
#include <src/model/song_with_metadata.hpp>
#include <src/model/app_config.hpp>

template <typename T>
std::unique_ptr<T> msw::data::ProductionAccessor<T>::initial_load_function() {
  return std::unique_ptr<T>();
}

template <typename T>
void msw::data::ProductionAccessor<T>::persist(T* item) {
  item->serialize(path_for_disk_storage_);
}

template <typename T>
msw::data::ProductionAccessor<T>::ProductionAccessor(model::AppConfig& app_config,
                                                     const std::string& path_for_disk_storage)
  : Accessor(&app_config_),
    app_config_(app_config),
    path_for_disk_storage_(path_for_disk_storage) {
}


// template class msw::data::ProductionAccessor<msw::model::SongList>;
template class msw::data::ProductionAccessor<msw::model::SongWithMetadata>;

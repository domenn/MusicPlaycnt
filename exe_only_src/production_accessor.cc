#include "production_accessor.hpp"

#include <src/misc/spd_logging.hpp>
#include <src/model/app_config.hpp>
#include <src/model/song_with_metadata.hpp>
#include <src/win/winapi_exceptions.hpp>

template <typename T>
std::unique_ptr<T> msw::data::ProductionAccessor<T>::initial_load_function() {
  static_assert(!std::is_same<T, msw::model::AppConfig>::value && !std::is_same<T, const msw::model::AppConfig>::value,
                "This function doesn't support config. I handle it seperately. Historic reasons I guess. Didn't "
                "refactor this part.");
  try {
    return std::make_unique<T>(Serializable::from_file<T>(path_for_disk_storage_));
  } catch (const msw::exceptions::ErrorCode& err) {
    if (err.is_enoent()) {
      SPDLOG_LOGGER_INFO(L_MSW_EVTS, "File {} doesn't exist yet. Creating new.", path_for_disk_storage_);
      return std::make_unique<T>();
    }
    throw;
  }
}

template <typename T>
void msw::data::ProductionAccessor<T>::persist(T* item) {
  item->serialize(path_for_disk_storage_);
}

template <typename T>
msw::data::ProductionAccessor<T>::ProductionAccessor(const std::string& path_for_disk_storage)
    : path_for_disk_storage_(path_for_disk_storage) {}

template class msw::data::ProductionAccessor<msw::model::SongList>;
template class msw::data::ProductionAccessor<msw::model::SongWithMetadata>;

#include "accessor.hpp"

#include <src/model/song_list.hpp>
#include <src/model/song_with_metadata.hpp>

template <typename T>
void msw::data::Accessor<T>::write(std::function<void(T*)> mutator) {
  if (!item_) {
    item_ = initial_load_function();
  }
  mutator(item_.get());
  persist(item_.get());
}

template <typename T>
T* msw::data::Accessor<T>::read() {
  if (!item_) {
    item_ = initial_load_function();
  }
  return item_.get();
}

template <typename T>
void msw::data::Accessor<T>::replace(T* replacement_item) {
  if (!item_) {
    item_ = std::make_unique<T>(std::move(*replacement_item));
  } else {
    *item_ = std::move(*replacement_item);
  }
  persist(item_.get());
}

template <typename T_item>
void msw::data::Accessor<T_item>::replace(T_item&& replacement_item) {
  replace(&replacement_item);
}

template class msw::data::Accessor<msw::model::SongList>;
template class msw::data::Accessor<msw::model::SongWithMetadata>;

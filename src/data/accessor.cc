#include "accessor.hpp"

#include <src/model/song_list.hpp>
#include <src/model/song_with_metadata.hpp>

template <typename T>
msw::data::Accessor<T>::Accessor(void* data)
//   : data_(data)
{
}

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
  }else {
    *item_ = std::move(*replacement_item);
  }
  persist(item_.get());
}

template class msw::data::Accessor<msw::model::SongList>;
template class msw::data::Accessor<msw::model::SongWithMetadata>;

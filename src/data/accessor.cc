#include "accessor.hpp"

#include <src/model/song_list.hpp>
#include <src/model/song_with_metadata.hpp>

template <typename T>
msw::data::Accessor<T>::Accessor(void* data,
                                 std::function<void(void*, T*)> persist_function,
                                 std::function<std::unique_ptr<T>(void*)> initial_load_function)
  : data_(data),
    persist_function_(std::move(persist_function)),
    initial_load_function_(std::move(initial_load_function)) {
}

template <typename T>
void msw::data::Accessor<T>::write(std::function<void(T*)> mutator) {
  if (!item_) {
    item_ = initial_load_function_(data_);
  }
  mutator(item_.get());
  persist_function_(data_, item_.get());
}

template <typename T>
T* msw::data::Accessor<T>::read() {
  if (!item_) {
    item_ = initial_load_function_(data_);
  }
  return item_.get();
}

template class msw::data::Accessor<msw::model::SongList>;
template class msw::data::Accessor<msw::model::SongWithMetadata>;

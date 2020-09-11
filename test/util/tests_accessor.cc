#include "tests_accessor.hpp"

#include <src/data/pointers_to_globals.hpp>
#include <src/model/song_with_metadata.hpp>

namespace msw::pg {
data::DataForTests* data_for_tests;
}  // namespace msw::pg

template <typename T>
std::unique_ptr<T> msw::data::TestsAccessor<T>::initial_load_function() {
  return std::make_unique<T>();
}

template <typename T>
void msw::data::TestsAccessor<T>::persist(T* item) {
  inst_data_.serialized_ = item->serialize_to_str();
}

template <typename T>
msw::data::TestsAccessor<T>::TestsAccessor() : Accessor<T>() {
  msw::pg::data_for_tests = &inst_data_;
}

template class msw::data::TestsAccessor<msw::model::SongWithMetadata>;

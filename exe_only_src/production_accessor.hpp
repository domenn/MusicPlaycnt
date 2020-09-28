#pragma once
#include "production_accessor.hpp"
#include <src/data/accessor.hpp>
#include <src/model/song_list.hpp>
#include <src/model/song_with_metadata.hpp>

namespace msw::model {
class AppConfig;
}  // namespace msw::model

namespace msw::data {

// struct ProductionAccessorData {
//
//};

template <typename T>
class ProductionAccessor : public msw::data::Accessor<T> {
  const std::string& path_for_disk_storage_;

  std::unique_ptr<T> initial_load_function() override;
  void persist(T* item) override;

 public:
  ProductionAccessor(const std::string& path_for_disk_storage);
};
}  // namespace msw::data
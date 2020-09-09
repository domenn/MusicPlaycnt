#pragma once
#include <src/data/accessor.hpp>
#include <src/model/song_list.hpp>
#include <src/model/song_with_metadata.hpp>

#include "production_accessor.hpp"

namespace msw::model {
class AppConfig;
}  // namespace msw::model

namespace msw::data {

// struct ProductionAccessorData {
//
//};

template <typename T>
class ProductionAccessor : public msw::data::Accessor<T> {
  model::AppConfig& app_config_;
  const std::string& path_for_disk_storage_;

  std::unique_ptr<T> initial_load_function() override;
  void persist(T* item) override;

 public:
  ProductionAccessor(model::AppConfig& app_config
  , const std::string& path_for_disk_storage
  );
};
}  // namespace msw::data
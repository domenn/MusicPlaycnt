#pragma once
#include <src/data/accessor.hpp>

#include "data_for_tests.hpp"

namespace msw::data {

template <typename T>
class TestsAccessor : public msw::data::Accessor<T> {
  DataForTests inst_data_;

  std::unique_ptr<T> initial_load_function() override;
  void persist(T* item) override;

 public:
  const DataForTests& inst_data() const { return inst_data_; }

  TestsAccessor();
};
}  // namespace msw::data

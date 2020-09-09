#pragma once

#include <memory>
#include <functional>

namespace msw::data {

template <typename T_item>
class Accessor {

  std::unique_ptr<T_item> item_;
  // NOTE: Probably useless. In phase of phasing out.
  // void* data_;

  virtual std::unique_ptr<T_item> initial_load_function()= 0;
  virtual void persist(T_item* item) = 0;

public:
  explicit Accessor(void* data);
  virtual ~Accessor() = default;

  void write(std::function<void(T_item*)> mutator);
  T_item* read();

  // void* data() const { return data_; }
  void replace(T_item* replacement_item);

  //  virtual msw::model::SongWithMetadata working_item();
  //  virtual void persist();

};
} // namespace msw::data

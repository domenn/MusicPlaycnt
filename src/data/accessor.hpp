#pragma once

#include <memory>
#include <functional>

namespace msw::data {

template <typename T>
class Accessor {

  std::unique_ptr<T> item_;

  void* data_;
  std::function<void(void*, T*)> persist_function_;
  std::function<std::unique_ptr<T>(void*)> initial_load_function_;


public:
  Accessor(void* data,
           std::function<void(void*, T*)> persist_function,
           std::function<std::unique_ptr<T>(void*)> initial_load_function);

  void write(std::function<void(T*)> mutator);
  T* read();

  void* data() const { return data_; }


  //  virtual msw::model::SongWithMetadata working_item();
  //  virtual void persist();

};
} // namespace msw::data

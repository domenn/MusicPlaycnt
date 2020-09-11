#pragma once
#include <string>

namespace msw::model {
enum class ActionType {
  PLAY = 0,
  PAUSE = 1,
  STOP = 2,

  NONE = 127
};

ActionType from_foo_string(const std::string& foobar_string);

}  // namespace msw::model

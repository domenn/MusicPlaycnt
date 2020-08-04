#include "action_type.hpp"
#include "src/win/winapi_exceptions.hpp"

msw::model::ActionType msw::model::from_foo_string(const std::string& foobar_string) {
  if (foobar_string == "stopped") {
    return ActionType::STOP;
  }
  if (foobar_string == "paused") {
    return ActionType::PAUSE;
  }
  if (foobar_string == "playing") {
    return ActionType::PLAY;
  }
  throw exceptions::ApplicationError(("Cannot parse: " + foobar_string).c_str(), MSW_TRACE_ENTRY_CREATE);
}
#pragma once
#include <string>

namespace msw {
class StringProvider {
  int seq = 0;

 public:
  std::string get_str(std::string&& prefix = "");

  std::tuple<std::string, std::string, std::string, std::string> four_strings(std::string&& prefix = "");
};

std::string p_mk(std::string&& token);
}  // namespace msw

#pragma once

#include <src/win/windows_headers.hpp>

#include <external/nlohmann/json.hpp>
#include <string>

class LegacyJsonImporter {
  void iterate_and_import(const nlohmann::json &json);

 public:
  ~LegacyJsonImporter();

  LegacyJsonImporter(std::string &&json_input_path);
  LegacyJsonImporter(std::istream &&input_stream);
};

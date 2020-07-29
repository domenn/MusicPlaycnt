#pragma once
#include <string>

namespace msw::helpers {
class ProtobufReadWriteDisk {
public:
  static void write_message(std::string disk_file_path);
};
}

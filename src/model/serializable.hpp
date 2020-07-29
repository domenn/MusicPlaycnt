#pragma once

#include <src/win/windows_headers.hpp>
#include <string>


namespace google {
namespace protobuf {
class Message;
namespace io {
class ZeroCopyOutputStream;
}
}
}

namespace msw {

class Serializable {

public:
  static void serialize(const google::protobuf::Message&, google::protobuf::io::ZeroCopyOutputStream& output_stream);
  void open_file_for_writing(const std::string& path);
  };
}

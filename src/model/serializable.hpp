#pragma once

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <string>

namespace google {
namespace protobuf {
class Message;

namespace io {
class ZeroCopyOutputStream;
class FileOutputStream;
}  // namespace io
}  // namespace protobuf
}  // namespace google

namespace msw {

class Serializable {
 protected:
  google::protobuf::Message* underlying_object_{};

  explicit Serializable(google::protobuf::Message* underlying_object);
  Serializable() = default;

 public:
  std::string serialize();
  void serialize(std::string output_path);
  template <typename T>
  static T from_file(std::string const& in_path);

  template <typename proto_message_type>
  static std::string serialize(const proto_message_type& msg);

  template <typename T>
  static T from_string(const std::string& contents) {
    T empty_proto_thing;
    deserialize_string_impl(contents, &empty_proto_thing);
    return empty_proto_thing;
  }

 private:
  google::protobuf::io::FileOutputStream open_file_for_writing(const std::string& path);
  static google::protobuf::io::FileInputStream open_file_for_reading(const std::string& path);
  void serialize_impl(google::protobuf::io::ZeroCopyOutputStream& output_stream);
  void read_impl(google::protobuf::io::ZeroCopyInputStream& input_stream);
  static void deserialize_string_impl(const std::string& contents, google::protobuf::Message* msg);
};
}  // namespace msw

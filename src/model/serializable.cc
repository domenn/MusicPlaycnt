#include "serializable.hpp"
#include <google/protobuf/text_format.h>


void msw::Serializable::serialize(const google::protobuf::Message& m,
                                         google::protobuf::io::ZeroCopyOutputStream& output_stream) {
  // google::protobuf::TextFormat::Print(reinterpret_cast<google::protobuf::Message&>(*this), &output_stream);
  google::protobuf::TextFormat::Print(m, &output_stream);
}

void msw::Serializable::open_file_for_writing(const std::string& path) {

  //  cp_open_lw(filename, O_WRONLY | O_CREAT | O_TRUNC, get_wr_pmode(), OpenModeWin::BINARY);
  //  if (new_file == -1) {
  //    throw ErrCodeException(fmt::format("At: {}{}{}{}\n Cannot write file: {}. open() error (code {{}} - {}): {}",
  //                                       D_U_M_FUNC_FILE_LINE_TRACE_IN_FMT,
  //                                       filename,
  //                                       errnoname(my_errno),
  //                                       d_common::safe::strerror(my_errno)),
  //                           my_errno);
  //  }
  //  google::protobuf::io::FileOutputStream proto_os(new_file);
  //  proto_os.SetCloseOnDelete(true);
  //  google::protobuf::TextFormat::Print(message, &proto_os);

}

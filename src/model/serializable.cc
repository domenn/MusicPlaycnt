#include "serializable.hpp"

#include <google/protobuf/text_format.h>

#include <src/model/app_config.hpp>

#include "src/misc/open_wrap.hpp"
#include "src/protobufs/songs.pb.h"
#include "src/win/winapi_exceptions.hpp"

constexpr auto get_wr_pmode() {
#ifdef _WIN32
  return S_IWRITE;
#else
  return S_IRWXU | S_IREAD | S_IRGRP | S_IROTH;
#endif
}

std::string msw::Serializable::serialize() {
  std::string returning;
  google::protobuf::TextFormat::PrintToString(*underlying_object_, &returning);
  return returning;
}

void msw::Serializable::serialize(std::string output_path) {
  auto os = open_file_for_writing(output_path);
  os.SetCloseOnDelete(true);
  serialize_impl(os);
  // os.Close();
}

google::protobuf::io::FileOutputStream msw::Serializable::open_file_for_writing(const std::string& path) {
  auto [new_file, my_errno] =
      cp_open_lw(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, get_wr_pmode(), OpenModeWin::BINARY);
  if (new_file == -1) {
#ifdef _WIN32
    using thrown_e = msw::exceptions::WinApiError;
#else
    using thrown_e = msw::exceptions::ErrorCode;
#endif
    throw thrown_e(my_errno, M_PROTOBUF_OPEN_FILE_IMPL, {__LINE__, __FUNCTION__, __FILE__});
  }
  return google::protobuf::io::FileOutputStream(new_file);
}

google::protobuf::io::FileInputStream msw::Serializable::open_file_for_reading(const std::string& path) {
  auto [existing_file, my_errno] = cp_open_lw(path.c_str(), O_RDONLY);
  if (existing_file == -1) {
    throw msw::exceptions::ErrorCode(my_errno, M_PROTOBUF_OPEN_FILE_IMPL, {__LINE__, __FUNCTION__, __FILE__});
  }
  return google::protobuf::io::FileInputStream(existing_file);
}

void msw::Serializable::serialize_impl(google::protobuf::io::ZeroCopyOutputStream& output_stream) {
  if (!google::protobuf::TextFormat::Print(*underlying_object_, &output_stream)) {
    throw msw::exceptions::ApplicationError("Protobuf print failed", MSW_TRACE_ENTRY_CREATE);
  }
}

/*

Alternative: all polymorphic

template <typename stream_t>
  void serialize_impl(
      google::protobuf::io::ZeroCopyOutputStream& output_stream);

template <typename proto_t>
template <typename stream_t>
void msw::Serializable<proto_t>::serialize_impl(google::protobuf::io::ZeroCopyOutputStream& output_stream) {

 */

// template class msw::Serializable<msw_proto_song::Song>;
// template class msw::Serializable<msw_proto_cfg::PlaycntConfig>;

msw::Serializable::Serializable(google::protobuf::Message* underlying_object) : underlying_object_(underlying_object) {}

void msw::Serializable::read_impl(google::protobuf::io::ZeroCopyInputStream& input_stream) {
  if (!google::protobuf::TextFormat::Parse(&input_stream, underlying_object_)) {
    throw msw::exceptions::ApplicationError("Protobuf read failed", MSW_TRACE_ENTRY_CREATE);
  }
}

template <typename T>
T msw::Serializable::from_file(std::string const& in_path) {
  T serializable;
  auto read_from = open_file_for_reading(in_path);
  read_from.SetCloseOnDelete(true);
  serializable.read_impl(read_from);
  return serializable;
}

template <typename proto_message_type>
std::string msw::Serializable::serialize(const proto_message_type& msg) {
  std::string returning;
  google::protobuf::TextFormat::PrintToString(msg, &returning);
  return returning;
}

void msw::Serializable::deserialize_string_impl(const std::string& contents, google::protobuf::Message* msg) {
  std::istringstream istringstr(contents);
  google::protobuf::io::IstreamInputStream proto_iss(&istringstr);
  google::protobuf::TextFormat::Parse(&proto_iss, msg);
}

template msw::model::AppConfig msw::Serializable::from_file<msw::model::AppConfig>(std::string const& in_path);
template std::string msw::Serializable::serialize<msw_proto_song::Song>(const msw_proto_song::Song&);
template std::string msw::Serializable::serialize<msw_proto_song::SongWithMetadata>(
    const msw_proto_song::SongWithMetadata&);

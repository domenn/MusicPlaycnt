#include "utilities.hpp"

#include <src/data/pointers_to_globals.hpp>
#include <src/model/song.hpp>
#include <src/win/encoding.hpp>
#include <src/win/winapi_exceptions.hpp>
#include <src/win/windows_headers.hpp>

#include "consts.hpp"

#ifdef _WIN32
#include <shellapi.h>
#else
#define THROW_IF_ERROR_LINUX                                                                                \
  if (!was_success) {                                                                                       \
    assert(0 && "Untested here. finalizer unique ptr -- needs debugging and custom deleter.");              \
    if (error != nullptr) {                                                                                 \
      std::unique_ptr<GError, decltype(&g_error_free)> finalizer(error, g_error_free);                      \
      throw d_common::exc::OsApiException(                                                                  \
          fmt::format("Error: {}{}{}{}; More info: GQuark domain={};; gint code={};; "                      \
                      "gchar* message={}",                                                                  \
                      D_U_M_FUNC_FILE_LINE_TRACE_IN_FMT,                                                    \
                      error->domain,                                                                        \
                      error->code,                                                                          \
                      error->message));                                                                     \
    } else {                                                                                                \
      throw d_common::exc::ProcessRunnerException(fmt::format(                                              \
          "Unknown error during linux app_exec operation!\n {}{}{}{}", D_U_M_FUNC_FILE_LINE_TRACE_IN_FMT)); \
    }                                                                                                       \
  }
#endif

namespace co = msw::consts::cmdoptions;

std::string msw::helpers::Utilities::file_in_app_folder(std::string file_name) {
  while (file_name[0] == '\\' || file_name[0] == '/') {
    file_name.erase(file_name.begin());
  }
  return app_folder() + '/' + file_name;
}

void create_folder(const std::string& file_name) {
#if defined(_WIN32)
  const auto windows_path = msw::encoding::utf8_to_utf16(file_name.c_str());
  if (_wmkdir(windows_path.c_str())) {
    throw msw::exceptions::WinApiError(errno, "_wmkdir", MSW_TRACE_ENTRY_CREATE);
  }
#else
  if (mkdir(file_name.c_str(), mode)) {
    throw msw::exceptions::ErrorCode(errno, "mkdir", MSW_TRACE_ENTRY_CREATE);
  }
#endif
}

std::string msw::helpers::Utilities::file_in_app_folder_with_creating_app_folder(std::string file_name) {
  try {
    create_folder(app_folder());
  } catch (const msw::exceptions::ErrorCode& ec) {
    if (!ec.is_already_exists()) {
      throw;
    }
  }
  return file_in_app_folder(std::move(file_name));
}

std::string msw::helpers::Utilities::get_parent_folder(const std::string& path) {
  return get_parent_folder_and_filename(path).first;
}

template <typename char_t>
std::basic_string<char_t> get_file_separator() {
  if constexpr (std::is_same<char_t, wchar_t>::value) {
    return L"\\/";
  } else {
    return "\\/";
  }
}

template <typename char_t>
std::pair<std::basic_string<char_t>, std::basic_string<char_t>> msw::helpers::Utilities::get_parent_folder_and_filename(
    const std::basic_string<char_t>& path) {
  try {
    const auto idx = path.find_last_of(get_file_separator<char_t>());
    return std::make_pair(path.substr(0, idx), path.substr(idx + 1));
  } catch (const std::exception& x) {
    const std::string msg = "Getting parent folder of " + encoding::ensure_utf8(std::move(path)) +
                            " failed. Exception " + typeid(x).name() + " was thrown:\n " + x.what();
    throw msw::exceptions::ApplicationError(msg.c_str(), MSW_TRACE_ENTRY_CREATE);
  }
}

void msw::helpers::Utilities::start_non_executable_file(const std::string& path) {
#ifdef _WIN32
  std::wstring filename_win_frindly = encoding::utf8_to_utf16(path.c_str());
  ShellExecuteW(0, 0, filename_win_frindly.c_str(), 0, 0, SW_SHOW);
#else
  gboolean was_success;
  GError* error = nullptr;

#if GLIB_CHECK_VERSION(2, 36, 0)
  D_DLOGT("Glib is quite new ... no need to g_type_init()");
#else
  g_type_init();
#endif

  was_success = g_app_info_launch_default_for_uri(
      ("file://" + std::filesystem::canonical(std::filesystem::path(path)).generic_string()).c_str(), NULL, &error);
  THROW_IF_ERROR_LINUX
#endif
}

std::string msw::helpers::Utilities::get_thread_description() {
#ifdef _DLL
  wchar_t* data;
  std::string return_val{};
  HRESULT hr = GetThreadDescription(GetCurrentThread(), &data);
  if (SUCCEEDED(hr)) {
    return_val = encoding::utf16_to_utf8(data);
    LocalFree(data);
  }
  return return_val;
#else
  return "notImplemented";
#endif
}

msw::helpers::CmdParse::ArgcArgv msw::helpers::CmdParse::ArgcArgv::from_wstring(const wchar_t* wins_cmdline) {
  int num_args;
  std::vector<char> raw_data;
  std::vector<const char*> pointers;

  SPDLOG_TRACE("Received commandline {}", wins_cmdline);

  wchar_t** output = CommandLineToArgvW(wins_cmdline, &num_args);
  pointers.push_back(nullptr);
  for (int i = 0; i < num_args; ++i) {
    std::string temporary_str = msw::encoding::utf16_to_utf8(output[i]);
    std::copy(temporary_str.begin(), temporary_str.end(), std::back_inserter(raw_data));
    raw_data.emplace_back('\0');
    pointers.push_back(pointers[0] + raw_data.size());
  }
  LocalFree(output);
  pointers.pop_back();
  for (auto& pointer : pointers) {
    pointer = (&*raw_data.begin()) + reinterpret_cast<intptr_t>(pointer);
  }
  return ArgcArgv{num_args, pointers.data(), std::move(raw_data), std::move(pointers)};
}

cxxopts::Options msw::helpers::CmdParse::create_options() {
  cxxopts::Options options(msw::consts::PROGRAM_NAME_SHORT, msw::consts::CLI_PROGRAM_DESCTIPTION);
  options.add_options()(co::listen_B, "Listener for file. Tray app.");
  options.add_options()(co::artist_B, "Artist of current song.", cxxopts::value<std::string>()->default_value(""));
  options.add_options()(co::album_B, "Album of current song.", cxxopts::value<std::string>());
  options.add_options()(co::title_B, "Title of current song.", cxxopts::value<std::string>()->default_value(""));
  options.add_options()(co::path_B, "Path of current song.", cxxopts::value<std::string>()->default_value(""));
  return options;
}

void msw::helpers::CmdParse::try_get_throw(const std::string& parameter_name,
                                           const char* const as_name,
                                           const std::exception& thrown_x) {
  throw msw::exceptions::ApplicationError(
      ("Commandline issue getting " + parameter_name + " of type " + as_name + ". Exception thrown \"" +
       thrown_x.what() + "\"\n  Of type: \"" + typeid(thrown_x).name())
          .c_str(),
      MSW_TRACE_ENTRY_CREATE);
}

msw::helpers::CmdParse::CmdParse(const int argc, const char** argv)
    : argc_argv_{argc, argv},
      options_(create_options()),
      result_(options_.parse(const_cast<int&>(argc_argv_.argc_), const_cast<char**&>(argc_argv_.argv_))) {}

#ifdef _WIN32
msw::helpers::CmdParse::CmdParse(const wchar_t* lpCmdLine)
    : argc_argv_(ArgcArgv::from_wstring(lpCmdLine)),
      options_(create_options()),
      result_(options_.parse(const_cast<int&>(argc_argv_.argc_), const_cast<char**&>(argc_argv_.argv_))) {}
#endif

bool msw::helpers::CmdParse::is_listen() const { return result_[co::listen_LO].as<bool>(); }

msw::helpers::ParseSongItems msw::helpers::CmdParse::song_data() const {
  return {try_get<std::string>(co::artist_LO),
          try_get<std::string>(co::album_LO),
          try_get<std::string>(co::title_LO),
          try_get<std::string>(co::path_LO)};
}

namespace msw::pg {
data::Accessor<msw::model::SongList>* song_list;
data::Accessor<msw::model::SongWithMetadata>* handled_song;
msw::model::AppConfig* app_config;
}  // namespace msw::pg

template std::pair<std::string, std::string> msw::helpers::Utilities::get_parent_folder_and_filename(
    const std::string& path);
template std::pair<std::wstring, std::wstring> msw::helpers::Utilities::get_parent_folder_and_filename(
    const std::wstring& path);

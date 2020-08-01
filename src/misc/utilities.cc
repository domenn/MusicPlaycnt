#include "utilities.hpp"

#include <src/win/windows_headers.hpp>
#include <src/win/winapi_exceptions.hpp>

#include <sago/platform_folders.h>
#include "consts.hpp"
#include "src/win/encoding.hpp"

#ifdef _WIN32
#include <shellapi.h>
#else
#define THROW_IF_ERROR_LINUX                                                             \
    if (!was_success) {                                                                  \
         assert(0 && "Untested here. finalizer unique ptr -- needs debugging and custom deleter.");          \
        if (error != nullptr) {                                                          \
                  std::unique_ptr<GError, decltype(&g_error_free)> finalizer(error, g_error_free);    \
            throw d_common::exc::OsApiException(fmt::format(                              \
                "Error: {}{}{}{}; More info: GQuark domain={};; gint code={};; "         \
                "gchar* message={}",                                                     \
                D_U_M_FUNC_FILE_LINE_TRACE_IN_FMT, error->domain, error->code,           \
                error->message));                                                        \
        } else {                                                                         \
            throw d_common::exc::ProcessRunnerException(                                          \
                fmt::format("Unknown error during linux app_exec operation!\n {}{}{}{}", \
                            D_U_M_FUNC_FILE_LINE_TRACE_IN_FMT));                         \
        }                                                                                \
    }
#endif

std::string msw::helpers::Utilities::app_folder() {
  return sago::getConfigHome() + '/' + consts::PROGRAM_NAME_SHORT;
}

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
    if(mkdir(file_name.c_str(),mode)){
        throw msw::exceptions::ErrorCode (errno, "mkdir", MSW_TRACE_ENTRY_CREATE);
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
    const std::string msg = "Getting parent folder of " + encoding::ensure_utf8(std::move(path)) + " failed. Exception "
                            + typeid(x).name() + " was thrown:\n " + x.what();
    throw msw::exceptions::ApplicationError(msg.c_str(), MSW_TRACE_ENTRY_CREATE);
  }
}

void msw::helpers::Utilities::start_non_executable_file(const std::string& path) {
#ifdef _WIN32
  std::wstring filename_win_frindly = encoding::utf8_to_utf16(path.c_str());
  ShellExecuteW(0, 0, filename_win_frindly.c_str(), 0, 0, SW_SHOW);
#else
  gboolean was_success;
  GError *error = nullptr;

#if GLIB_CHECK_VERSION(2, 36, 0)
  D_DLOGT("Glib is quite new ... no need to g_type_init()");
#else
  g_type_init();
#endif

  was_success = g_app_info_launch_default_for_uri(
      ("file://" +
          std::filesystem::canonical(std::filesystem::path(path)).generic_string())
          .c_str(),
      NULL, &error);
  THROW_IF_ERROR_LINUX
#endif
}

template std::pair<std::string, std::string> msw::helpers::Utilities::get_parent_folder_and_filename(
    const std::string& path);
template std::pair<std::wstring, std::wstring> msw::helpers::Utilities::get_parent_folder_and_filename(
    const std::wstring& path);

#pragma once
#include <string>

namespace msw::windows {

/**
 * Get some error info from WinApi.
 *
 * \param error_code returned by GetLastError().
 * \param lpszFunction user defined string, usually function name.
 * \return human-readable message.
 */
std::wstring format_windows_error_w(uint32_t error_code, const wchar_t* lpszFunction);

/**
 * Similiar to {@link format_windows_error_w}, but custom error_code cannot be passed. GetLastError gets called automatically.
 *
 * \param lpszFunction user defined string, usually function name.
 * \return human-readable message.
 */
std::wstring format_get_last_error_w(const wchar_t* lpszFunction);

inline std::string format_windows_error(uint32_t error_code, const char* function);

} // namespace d_common::windows

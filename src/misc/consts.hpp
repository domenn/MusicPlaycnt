#pragma once

#define A_LOCALLY_SCOPED_M_CPP_CHAR_CONST(nm, str) constexpr static char nm[] = str;
#define W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(nm, str) constexpr static wchar_t nm##W[] = L##str;

#define LOCALLY_SCOPED_M_AW_CPP_CHAR_CONST(nm, str) \
  A_LOCALLY_SCOPED_M_CPP_CHAR_CONST(nm, str)        \
  W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(nm, str)

namespace msw::consts {

LOCALLY_SCOPED_M_AW_CPP_CHAR_CONST(PROGRAM_NAME_SHORT, "musicPlaycnt2")
LOCALLY_SCOPED_M_AW_CPP_CHAR_CONST(CLI_PROGRAM_DESCTIPTION, "Complicated music tagger and count tracker.")

namespace ver {
constexpr static wchar_t PROGRAM_NAME_AND_VERSION[] =
    L"Music playcount tracker\n"
    L"Version 2.0.0 alpha\n"
#ifndef NDEBUG
    L" Debug"
#else
    L"Release"
#endif
    L" Build";
W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(VERSION_HEADER, "About")
}  // namespace ver

namespace registry {
W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(LISTENED_KEY,
                                  R"(Software\Microsoft\Windows NT\CurrentVersion\Windows\SessionDefaultDevices)")

W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(KEY_TO_CHANGE, R"(Software\Microsoft\Windows NT\CurrentVersion\Windows)")
W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(VALUE_TO_CHANGE, "device")

}  // namespace registry

namespace win {
W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(WIN_HEADER_TXT, "TS keep-up remote session printers.")
W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(WIN_CLASS_NAME, "TsCitricRdcPrintRegistryJdkFixPlaceholdr.")
}  // namespace win

namespace interact {
W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(MSG_ERROR_HEADER, "Error")
W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(MSG_WORKER_THREAD_DIED,
                                  "Registry listener thread has stopped unexpectedly. The application needs to stop.")
W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(MSG_OBSERVER_NO_RELEASE, "Observer mode is only available in debug build!")
W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(MSG_CRASH_UNEXPECTED_EXCEPTION,
                                  "Unexpected exception:\n %1\nApplication will now exit.")

}  // namespace interact
}  // namespace msw::consts
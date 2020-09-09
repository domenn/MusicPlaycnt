#pragma once

#define A_LOCALLY_SCOPED_M_CPP_CHAR_CONST(nm, str) constexpr static char nm[] = str;
#define W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(nm, str) constexpr static wchar_t nm##W[] = L##str;

#define LOCALLY_SCOPED_M_AW_CPP_CHAR_CONST(nm, str) \
  A_LOCALLY_SCOPED_M_CPP_CHAR_CONST(nm, str)        \
  W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(nm, str)

namespace msw::consts {

LOCALLY_SCOPED_M_AW_CPP_CHAR_CONST(PROGRAM_NAME_SHORT, "musicPlaycnt2")
LOCALLY_SCOPED_M_AW_CPP_CHAR_CONST(TESTS_DIR, "musicPlaycnt2__tests")
LOCALLY_SCOPED_M_AW_CPP_CHAR_CONST(CLI_PROGRAM_DESCTIPTION, "Complicated music tagger and count tracker.")
LOCALLY_SCOPED_M_AW_CPP_CHAR_CONST(CONFIG_FILENAME, "Playcntv2.config")
LOCALLY_SCOPED_M_AW_CPP_CHAR_CONST(DEFAULT_STATE_FILE_NAME, "app_state.txt")
LOCALLY_SCOPED_M_AW_CPP_CHAR_CONST(LISTENER_THREAD_NAME, "ThFileChListn")


// b	parses the month name, either full or abbreviated, e.g. Oct
// a	parses the name of the day of the week, either full or abbreviated, e.g. Fri	tm_wday
// d	parses the day of the month as a decimal number (range [01,31]), leading zeroes permitted but not required
LOCALLY_SCOPED_M_AW_CPP_CHAR_CONST(FOO_TIME_FMT, "%a %b %d %H:%M:%S %Y")


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
} // namespace ver


namespace win {
W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(WIN_HEADER_TXT, "TS keep-up remote session printers.")
W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(WIN_CLASS_NAME, "TsCitricRdcPrintRegistryJdkFixPlaceholdr.")
} // namespace win

namespace interact {
W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(MSG_ERROR_HEADER, "Error")
W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(MSG_WORKER_THREAD_DIED,
                                  "Registry listener thread has stopped unexpectedly. The application needs to stop.")
W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(MSG_OBSERVER_NO_RELEASE, "Observer mode is only available in debug build!")
W_LOCALLY_SCOPED_M_CPP_CHAR_CONST(MSG_CRASH_UNEXPECTED_EXCEPTION,
                                  "Unexpected exception:\n %1\nApplication will now exit.")

} // namespace interact
namespace cmdoptions {
#define TOSTR_(x) #x
#define TO_STR_COMMA(x, y) TOSTR_(x) "," #y
#define ONE_CMDOPTION_TEXT_CONSTANT(letter, full) \
      A_LOCALLY_SCOPED_M_CPP_CHAR_CONST(full##_B, TO_STR_COMMA(letter,full)) \
      A_LOCALLY_SCOPED_M_CPP_CHAR_CONST(full##_SH, #letter) \
      A_LOCALLY_SCOPED_M_CPP_CHAR_CONST(full##_LO, #full)

ONE_CMDOPTION_TEXT_CONSTANT(l, listen)
ONE_CMDOPTION_TEXT_CONSTANT(a, album)
ONE_CMDOPTION_TEXT_CONSTANT(t, title)
ONE_CMDOPTION_TEXT_CONSTANT(p, path)
ONE_CMDOPTION_TEXT_CONSTANT(g, artist)
ONE_CMDOPTION_TEXT_CONSTANT(o, op)
}
} // namespace msw::consts

#undef TOSTR_
#undef TO_STR_COMMA
#undef ONE_CMDOPTION_TEXT_CONSTANT
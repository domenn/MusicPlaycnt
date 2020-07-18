#pragma once

#define DIAG__HIDDEN_PRINT_XSTR(x) DIAG__HIDDEN_PRINT_STR(x)
#define DIAG__HIDDEN_PRINT_STR(x) #x

#define WIN32_LEAN_AND_MEAN

// Exclude serial communication api
#define NOCOMM

#ifndef NOMINMAX
#define NOMINMAX
#endif

#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOWH
#define NOKANJI
#define NOHELP
#define NODEFERWINDOWPOS
#define NOMCX
#define NOMETAFILE
#define NOGDI
#define NODRAWTEXT

// Look into SDKDDKVer.h to see what those numbers mean.
#define NTDDI_VERSION 0x0A000000
#ifndef _WIN32_WINNT
#define _WIN32_WINNT  0x0A00 // Windows 10
#else
#pragma message "WINNT already defined to " DIAG__HIDDEN_PRINT_XSTR(_WIN32_WINNT)
#endif
#define WINVER 0x0A00
#define _WIN32_IE 0x0A00

#include <Windows.h>


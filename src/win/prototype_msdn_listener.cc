#include "src/win/windows_headers.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include "winapi_exceptions.hpp"
#include "src/misc/utilities.hpp"

void RefreshDirectory(const wchar_t*);
void RefreshTree(const wchar_t*);
void WatchDirectory(const wchar_t*);

void listen_this(const wchar_t* fn) {
  WatchDirectory(fn);
}

void WatchDirectory(const wchar_t* lpDirInput) {
  DWORD dwWaitStatus;
  TCHAR lpDrive[4];
  TCHAR lpFile[_MAX_FNAME];
  TCHAR lpDir[_MAX_FNAME];
  TCHAR lpExt[_MAX_EXT];

  // const auto [dir, filename] = msw::helpers::Utilities::get_parent_folder_and_filename(std::wstring(lpDir));
  _tsplitpath_s(lpDirInput, lpDrive, 4, lpDir, _MAX_FNAME, lpFile, _MAX_FNAME, lpExt, _MAX_EXT);

  lpDrive[2] = (TCHAR)'\\';
  lpDrive[3] = (TCHAR)'\0';

  // Watch the directory for file creation and deletion.


  // return;


  HANDLE dwChangeHandle = FindFirstChangeNotification(
      lpDir,
      // directory to watch
      FALSE,
      // do not watch subtree
      FILE_NOTIFY_CHANGE_LAST_WRITE); // watch file name changes

  if (dwChangeHandle == INVALID_HANDLE_VALUE) {
    throw msw::exceptions::WinApiError(GetLastError(), "FindFirstChangeNotification", MSW_TRACE_ENTRY_CREATE);
  }

  // Make a final validation check on our handles.

  if ((dwChangeHandle == NULL)) {
    throw msw::exceptions::WinApiError(GetLastError(),
                                       "FindFirstChangeNotification",
                                       MSW_TRACE_ENTRY_CREATE,
                                       "ERROR: Unexpected NULL from FindFirstChangeNotification");
  }

  // Change notification is set. Now wait on both notification
  // handles and refresh accordingly.

  while (TRUE) {
    // Wait for notification.

    printf("\nWaiting for notification...\n");

    dwWaitStatus = WaitForSingleObject(dwChangeHandle,
                                       INFINITE);
    switch (dwWaitStatus) {
      case WAIT_OBJECT_0:

        // A file was created, renamed, or deleted in the directory.
        // Refresh this directory and restart the notification.

        RefreshDirectory(lpDir);
        if (FindNextChangeNotification(dwChangeHandle) == FALSE) {
          printf("\n ERROR: FindNextChangeNotification function failed.\n");
          ExitProcess(GetLastError());
        }
        break;

      case WAIT_OBJECT_0 + 1:

        // A directory was created, renamed, or deleted.
        // Refresh the tree and restart the notification.

        // RefreshTree(lpDrive);


      case WAIT_TIMEOUT:

        // A timeout occurred, this would happen if some value other
        // than INFINITE is used in the Wait call and no changes occur.
        // In a single-threaded environment you might not want an
        // INFINITE wait.

        printf("\nNo changes in the timeout period.\n");
        break;

      default:
        printf("\n ERROR: Unhandled dwWaitStatus.\n");
        ExitProcess(GetLastError());
        break;
    }
  }
}

void RefreshDirectory(const wchar_t* lpDir) {
  // This is where you might place code to refresh your
  // directory listing, but not the subtree because it
  // would not be necessary.

  _tprintf(TEXT("Directory (%s) changed.\n"), lpDir);
}

void RefreshTree(const wchar_t* lpDrive) {
  // This is where you might place code to refresh your
  // directory listing, including the subtree.

  _tprintf(TEXT("Directory tree (%s) changed.\n"), lpDrive);
}

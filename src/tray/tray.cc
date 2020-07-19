#include "../windows_headers.hpp"
#include "tray.hpp"
#include "../exc/winapi_exceptions.hpp"
#include "../windows_error_format.hpp"
#include "../../Res/resource.hpp"
#include <shellapi.h>
#include "../../msg_output.hpp"
#include "../../own_constants.hpp"
#include "../misc_windows_api.hpp"

using namespace msw;
using namespace tray;
namespace I1 = consts::interact;

LRESULT Tray::create_menu(Tray* tray) {
    if (tray == 0) {
        throw MAKE_DIAG_WIN_API_ERR(GetLastError(),
                                    L"((CREATESTRUCT*)lParam)->lpCreateParams)");
    }
    ITC_CPP_LOG(L"Loading menu from the resource.");
    const auto loaded_menu = LoadMenu(reinterpret_cast<HINSTANCE>(tray->hinstance_),
                                      MAKEINTRESOURCE(PRINTFX_MENU_TRAY_RIGHTCLICK));
    tray->menu_to_display_ = GetSubMenu(loaded_menu, 0);

    SetMenuDefaultItem(tray->menu_to_display_, 0, TRUE);
    return 0;
}

LRESULT on_version() {
    MessageBox(nullptr, consts::ver::PROGRAM_NAME_AND_VERSION,
               consts::ver::VERSION_HEADER, MB_ICONINFORMATION | MB_OK);
    return 0;
}

void Tray::on_exit() const {
    SendMessage(hwnd_, WM_CLOSE, 0, 0);
}

void Tray::handle_menu_choice(BOOL clicked) const {
    ITC_CPP_TRACE_LOG(L"Menu click: " + std::to_wstring(clicked));
    switch (clicked) {
    case PRINTFX_MENU_ENTRY_VERSION:
        on_version();
        break;
    case PRINTFX_MENU_ENTRY_EXIT:
        on_exit();
        break;
    }
}

LRESULT Tray::display_tray_menu(HWND hwnd) const {
    ITC_CPP_TRACE_LOG(L"buttondown ... Show context menu");
    // Get current mouse position.
    POINT curPoint;
    GetCursorPos(&curPoint);
    // should SetForegroundWindow according to original poster so the popup shows on top
    SetForegroundWindow(hwnd);
    // TrackPopupMenu blocks the app until TrackPopupMenu returns
    ITC_CPP_TRACE_LOG(L"calling TrackPopupMenu");
    // don't send me WM_COMMAND messages about this window, instead return the identifier
    // of the clicked menu item (TPM_NONOTIFY)
    BOOL clicked = TrackPopupMenu(menu_to_display_, TPM_RETURNCMD | TPM_NONOTIFY,
                                  curPoint.x, curPoint.y, 0, hwnd, nullptr);
    SendMessage(hwnd, WM_NULL, 0, 0);
    // send benign message to window to make sure the menu goes away.
    this->handle_menu_choice(clicked);
    return 0;
}

LRESULT Tray::handle_tray_icon_callback(HWND hwnd, UINT msg, WPARAM w_param,
                                        LPARAM l_param) {
    switch (l_param) {
    case WM_LBUTTONDBLCLK:
        return on_version();
    case WM_RBUTTONUP:
        return display_tray_menu(hwnd);
    default:
        // Return 0 to do nothing
        return 0;
    }
}

LRESULT CALLBACK Tray::wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    try {
        switch (msg) {
            // NOLINT(hicpp-multiway-paths-covered)
        case WM_CREATE: {
            auto* tray_ptr = reinterpret_cast<Tray*>(
                reinterpret_cast<CREATESTRUCTA*>(lParam)->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(tray_ptr));
            return create_menu(tray_ptr);
        }
        case TRAY_ICON_CALLBACK_ID:
            return get_tray_from_window(hwnd)->
                handle_tray_icon_callback(hwnd, msg, wParam, lParam);
            // intercept the hittest message.. making full body of
            // window draggable.
        case WM_NCHITTEST: {
            // http://www.catch22.net/tuts/tips
            // this tests if you're on the non client area hit test
            const auto uHitTest = DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
            if (uHitTest == HTCLIENT)
                return HTCAPTION;
            return uHitTest;
        }
        case CUSTOM_THREAD_IS_DOWN:
            MessageBox(
                nullptr, I1::MSG_WORKER_THREAD_DIED,
                I1::MSG_ERROR_HEADER,
                MB_ICONEXCLAMATION | MB_OK);
            get_tray_from_window(hwnd)->on_exit();
        }
    } catch (exceptions::WinApiError const& any) {
        MessageBox(
            nullptr,
            MiscWindowsApi::formatted_msg(I1::MSG_CRASH_UNEXPECTED_EXCEPTION,
                                          any.what_w().c_str())
            .c_str(),
            I1::MSG_ERROR_HEADER, MB_ICONERROR | MB_OK);
        get_tray_from_window(hwnd)->on_exit();
        return 0;
    }
    // Those two must be out of try catch to prevent them being handled - canceled and preventing app to be immortal.
    if (msg == WM_CLOSE) {
        return DestroyWindow(hwnd);
    }
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

NOTIFYICONDATA Tray::create_notifyicondata_structure(HICON icon) {
    NOTIFYICONDATA iconData;
    ZeroMemory(&iconData, sizeof(NOTIFYICONDATA));

    iconData.cbSize = sizeof(NOTIFYICONDATA);
    iconData.hWnd = hwnd_;
    iconData.uID = NOTIFICATION_STRUCTURE_UID;
    iconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    iconData.hIcon = icon;
    // Set callback number between WM_APP (0x8000) and 0xBFFF
    iconData.uCallbackMessage = TRAY_ICON_CALLBACK_ID;
    return iconData;
}


void Tray::make_window(int n_cmd_show, HICON h_icon) {
    WNDCLASSEX wc;
    ITC_CPP_LOG(L"Creating win");
    // Step 1: Registering the Window Class
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = wnd_proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hinstance_;
    wc.hIcon = h_icon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = consts::win::WIN_CLASS_NAME;
    wc.hIconSm = h_icon;

    if (!RegisterClassEx(&wc)) {
        MessageBox(nullptr, L"Window Registration Failed!", L"Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        throw MAKE_DIAG_WIN_API_ERR(GetLastError(), L"RegisterClassEx");
    }

    // Step 2: Creating the Window
    hwnd_ = CreateWindowEx(WS_EX_CLIENTEDGE, consts::win::WIN_CLASS_NAME,
                           consts::win::WIN_HEADER_TXT, WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT,
                           CW_USEDEFAULT, 260, 140, nullptr, nullptr, hinstance_, this);

    if (!hwnd_) {
        MessageBox(nullptr, L"Window Creation Failed!", L"Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        throw MAKE_DIAG_WIN_API_ERR(GetLastError(), L"CreateWindowEx");
    }

    ShowWindow(hwnd_, n_cmd_show);
    UpdateWindow(hwnd_);
    ITC_CPP_LOG(L"Created the hidden window.");
}

void Tray::setup_icon() {
    HICON h_trayicon = LoadIcon(hinstance_, MAKEINTRESOURCE(WRES_ICON_ICO));

    const wchar_t* windowHeader = consts::win::WIN_HEADER_TXT;
    make_window(SW_HIDE, h_trayicon);
    auto icon_data = create_notifyicondata_structure(h_trayicon);
    if (!Shell_NotifyIcon(NIM_ADD, &icon_data)) {
        throw MAKE_DIAG_WIN_API_ERR(GetLastError(), L"Shell_NotifyIcon");
    }
}

int tray::Tray::run_message_loop() {
    setup_icon();

    MSG msg;
    // Step 3: The Message Loop
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return static_cast<int>(msg.wParam);
}

void Tray::send_windows_message(UINT msg) const {
    if (!SendMessageW(hwnd_, msg, 0, 0)) {
        ITC_CPP_LOG_LAST_ERROR(L"SendMessageW");
    }
}

Tray::Tray(HINSTANCE hinstance)
    : hinstance_(hinstance) {
}
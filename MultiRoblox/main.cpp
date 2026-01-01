#define UNICODE
#define _UNICODE

#include <windows.h>
#include <shellapi.h>

#pragma comment(lib, "shell32.lib")

HANDLE g_hMutex = NULL;
NOTIFYICONDATA nid = {};

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_DESTROY)
    {
        if (g_hMutex)
        {
            ReleaseMutex(g_hMutex);
            CloseHandle(g_hMutex);
        }

        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        return 0;
    }

    if (msg == WM_USER + 1 && lParam == WM_RBUTTONUP)
    {
        HMENU menu = CreatePopupMenu();
        AppendMenuW(menu, MF_STRING, 1, L"Exit");

        POINT pt;
        GetCursorPos(&pt);
        SetForegroundWindow(hwnd);

        int cmd = TrackPopupMenu(
            menu,
            TPM_RETURNCMD | TPM_NONOTIFY,
            pt.x, pt.y,
            0, hwnd, NULL
        );

        if (cmd == 1)
            DestroyWindow(hwnd);

        DestroyMenu(menu);
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    // Acquire Roblox mutex
    g_hMutex = CreateMutexW(NULL, TRUE, L"ROBLOX_singletonMutex");
    if (!g_hMutex)
        return 0;

    // Register hidden window
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MultiRobloxTray";

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0,
        wc.lpszClassName,
        L"",
        WS_OVERLAPPEDWINDOW,
        0, 0, 0, 0,
        NULL, NULL, hInstance, NULL
    );

    // Tray icon
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_USER + 1;
    nid.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wcscpy_s(nid.szTip, L"MultiRoblox (Active)");

    Shell_NotifyIconW(NIM_ADD, &nid);

    // Message loop (idle = 0% CPU)
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}
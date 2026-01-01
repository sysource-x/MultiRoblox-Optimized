#define UNICODE
#define _UNICODE

#include <windows.h>
#include <shellapi.h>
#include <tlhelp32.h>

#pragma comment(lib, "shell32.lib")

HANDLE g_hMutex = NULL;
NOTIFYICONDATAW g_nid = {};
HWND g_hwnd = NULL;

// ===============================
// Count Roblox instances safely
// ===============================
int CountRobloxInstances()
{
    int count = 0;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return 0;

    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(entry);

    if (Process32FirstW(snapshot, &entry))
    {
        do
        {
            if (_wcsicmp(entry.szExeFile, L"RobloxPlayerBeta.exe") == 0)
            {
                count++;
            }
        } while (Process32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return count;
}

// ===============================
// Update tray tooltip text
// ===============================
void UpdateTrayTooltip()
{
    int count = CountRobloxInstances();

    wchar_t tooltip[128];
    if (count == 0)
        wcscpy_s(tooltip, L"MultiRoblox (no instances)");
    else if (count == 1)
        wcscpy_s(tooltip, L"MultiRoblox (1 instance)");
    else
        swprintf_s(tooltip, L"MultiRoblox (%d instances)", count);

    wcscpy_s(g_nid.szTip, tooltip);
    Shell_NotifyIconW(NIM_MODIFY, &g_nid);
}

// ===============================
// Window procedure
// ===============================
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_DESTROY)
    {
        if (g_hMutex)
        {
            ReleaseMutex(g_hMutex);
            CloseHandle(g_hMutex);
        }

        Shell_NotifyIconW(NIM_DELETE, &g_nid);
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

// ===============================
// Entry point (Tray App)
// ===============================
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
    wc.lpszClassName = L"MultiRobloxTrayClass";

    RegisterClassW(&wc);

    g_hwnd = CreateWindowExW(
        0,
        wc.lpszClassName,
        L"",
        WS_OVERLAPPEDWINDOW,
        0, 0, 0, 0,
        NULL, NULL, hInstance, NULL
    );

    // Setup tray icon
    g_nid.cbSize = sizeof(NOTIFYICONDATAW);
    g_nid.hWnd = g_hwnd;
    g_nid.uID = 1;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.uCallbackMessage = WM_USER + 1;
    g_nid.hIcon = LoadIconW(NULL, IDI_APPLICATION);

    wcscpy_s(g_nid.szTip, L"MultiRoblox");

    Shell_NotifyIconW(NIM_ADD, &g_nid);

    // Timer to refresh status every 2 seconds
    SetTimer(g_hwnd, 1, 2000, NULL);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_TIMER)
        {
            UpdateTrayTooltip();
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}feat(tray): show Roblox instance count in tray tooltip

/*
 * MultiRoblox - Ultra Optimized Version
 * Keeps ROBLOX singleton mutex alive with zero CPU usage
 */

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <Windows.h>
#include <iostream>
#include "color.h"

HANDLE g_hMutex = NULL;

BOOL WINAPI ConsoleHandler(DWORD signal)
{
	if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT)
	{
		if (g_hMutex)
		{
			ReleaseMutex(g_hMutex);
			CloseHandle(g_hMutex);
			g_hMutex = NULL;
		}
		ExitProcess(0);
	}
	return TRUE;
}

int main()
{
	SetConsoleCtrlHandler(ConsoleHandler, TRUE);

	g_hMutex = CreateMutexW(NULL, TRUE, L"ROBLOX_singletonMutex");

	if (!g_hMutex)
	{
		std::cout << dye::red("ERROR: Failed to create mutex.") << "\n";
		std::cin.get();
		return 1;
	}

	SetConsoleOutputCP(65001);

	std::cout << "\n";
	std::cout << dye::aqua("==============================================") << "\n";
	std::cout << dye::aqua("|") << dye::white("     MultiRoblox - Ultra Optimized Mode     ") << dye::aqua("|") << "\n";
	std::cout << dye::aqua("==============================================") << "\n\n";

	std::cout << dye::green("[OK] ") << dye::white("ROBLOX mutex acquired successfully.") << "\n";
	std::cout << dye::green("[OK] ") << dye::white("Multiple instances are now allowed.") << "\n\n";

	std::cout << dye::grey("Status: ") << dye::green("ACTIVE (0% CPU)") << "\n";
	std::cout << dye::grey("Press Ctrl+C to exit.") << "\n\n";

	// 🔥 ZERO CPU USAGE — no loop, no polling, no wakeups
	WaitForSingleObject(INVALID_HANDLE_VALUE, INFINITE);

	return 0;
}

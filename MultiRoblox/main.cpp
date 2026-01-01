/*
 * MultiRoblox - Ultimate Optimized Edition
 * Maximum compatibility, zero CPU usage, low-end friendly
 */

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <Windows.h>
#include <iostream>
#include <vector>
#include "color.h"

std::vector<HANDLE> g_mutexes;

BOOL WINAPI ConsoleHandler(DWORD signal)
{
	if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT)
	{
		for (HANDLE h : g_mutexes)
		{
			if (h)
			{
				ReleaseMutex(h);
				CloseHandle(h);
			}
		}
		ExitProcess(0);
	}
	return TRUE;
}

int main()
{
	SetConsoleCtrlHandler(ConsoleHandler, TRUE);
	SetConsoleOutputCP(65001);

	const wchar_t* mutexNames[] =
	{
		L"ROBLOX_singletonMutex",
		L"ROBLOX_singletonMutex_Legacy",
		L"ROBLOX_singletonMutex_1"
	};

	for (const auto& name : mutexNames)
	{
		HANDLE h = CreateMutexW(NULL, TRUE, name);
		if (h)
			g_mutexes.push_back(h);
	}

	if (g_mutexes.empty())
	{
		std::cout << dye::red("ERROR: Failed to acquire any ROBLOX mutex.") << "\n";
		std::cin.get();
		return 1;
	}

	std::cout << "\n";
	std::cout << dye::aqua("================================================") << "\n";
	std::cout << dye::aqua("|") << dye::white("  MultiRoblox - Ultimate Optimized Edition   ") << dye::aqua("|") << "\n";
	std::cout << dye::aqua("================================================") << "\n\n";

	std::cout << dye::green("[OK] ") << dye::white("ROBLOX mutexes acquired: ")
	          << dye::aqua(std::to_string(g_mutexes.size())) << "\n";

	std::cout << dye::grey("Status: ") << dye::green("ACTIVE (0% CPU)") << "\n";
	std::cout << dye::grey("Low-end systems: OPTIMIZED") << "\n\n";

	std::cout << dye::grey("Press Ctrl+C to exit.") << "\n\n";

	// 🔥 Absolute zero CPU usage
	WaitForSingleObject(INVALID_HANDLE_VALUE, INFINITE);
	return 0;
}
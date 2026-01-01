/*
 * MultiRoblox - Ultimate Optimized Edition
 * Maximum compatibility with modern Roblox launchers
 * Zero CPU usage, low-end friendly
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

std::vector<HANDLE> g_Mutexes;

BOOL WINAPI ConsoleHandler(DWORD signal)
{
	if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT)
	{
		for (HANDLE h : g_Mutexes)
		{
			if (h)
			{
				ReleaseMutex(h);
				CloseHandle(h);
			}
		}
		g_Mutexes.clear();
		std::cout << "\n" << dye::yellow("MultiRoblox shutting down. Mutexes released.") << "\n";
		ExitProcess(0);
	}
	return TRUE;
}

void AcquireMutex(const wchar_t* name)
{
	HANDLE h = CreateMutexW(NULL, TRUE, name);
	if (h)
	{
		g_Mutexes.push_back(h);
		std::wcout << L"[OK] Acquired mutex: " << name << L"\n";
	}
	else
	{
		std::wcout << L"[FAIL] Could not acquire mutex: " << name << L"\n";
	}
}

int main()
{
	SetConsoleCtrlHandler(ConsoleHandler, TRUE);
	SetConsoleOutputCP(65001);

	// Known Roblox mutexes (official + modern launchers)
	const wchar_t* mutexNames[] =
	{
		L"ROBLOX_singletonMutex",
		L"RobloxPlayerSingletonMutex",
		L"RobloxAppSingletonMutex"
	};

	for (const auto& name : mutexNames)
	{
		AcquireMutex(name);
	}

	if (g_Mutexes.empty())
	{
		std::cout << dye::red("ERROR: Failed to acquire any Roblox mutex.") << "\n";
		std::cin.get();
		return 1;
	}

	std::cout << "\n";
	std::cout << dye::aqua("========================================================") << "\n";
	std::cout << dye::aqua("|") << dye::white("   MultiRoblox - Ultimate Optimized Edition (CLI)   ") << dye::aqua("|") << "\n";
	std::cout << dye::aqua("========================================================") << "\n\n";

	std::cout << dye::green("[OK] ")
	          << dye::white("Total mutexes acquired: ")
	          << dye::aqua(std::to_string(g_Mutexes.size())) << "\n";

	std::cout << dye::grey("Status: ") << dye::green("ACTIVE") << "\n";
	std::cout << dye::grey("CPU Usage: ") << dye::green("0% (Idle Wait)") << "\n";
	std::cout << dye::grey("Compatibility: ") << dye::green("Official, Fishstrap, Bloxstrap") << "\n\n";

	std::cout << dye::grey("Press Ctrl + C to exit.") << "\n\n";

	// Zero CPU usage – correct infinite sleep
	Sleep(INFINITE);

	return 0;
}
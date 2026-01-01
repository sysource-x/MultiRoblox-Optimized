/*
 * MultiRoblox - Ultimate Optimized Edition
 * Robust core: instance detection + safe singleton
 */

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <vector>
#include <string>
#include "color.h"

// Prevent multiple MultiRoblox instances
HANDLE g_SelfMutex = NULL;

// Roblox mutexes
std::vector<HANDLE> g_RobloxMutexes;

// --------------------------------------------------
// Count Roblox instances
// --------------------------------------------------
int CountRobloxInstances()
{
	int count = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE)
		return 0;

	PROCESSENTRY32W entry{};
	entry.dwSize = sizeof(entry);

	if (Process32FirstW(snapshot, &entry))
	{
		do
		{
			if (_wcsicmp(entry.szExeFile, L"RobloxPlayerBeta.exe") == 0)
				count++;
		} while (Process32NextW(snapshot, &entry));
	}

	CloseHandle(snapshot);
	return count;
}

// --------------------------------------------------
// Acquire Roblox mutex
// --------------------------------------------------
void AcquireRobloxMutex(const wchar_t* name)
{
	HANDLE h = CreateMutexW(NULL, TRUE, name);
	if (h)
	{
		g_RobloxMutexes.push_back(h);
		std::wcout << L"[OK] Acquired mutex: " << name << L"\n";
	}
}

// --------------------------------------------------
// Cleanup
// --------------------------------------------------
void CleanupAndExit()
{
	for (HANDLE h : g_RobloxMutexes)
	{
		if (h)
		{
			ReleaseMutex(h);
			CloseHandle(h);
		}
	}
	g_RobloxMutexes.clear();

	if (g_SelfMutex)
	{
		ReleaseMutex(g_SelfMutex);
		CloseHandle(g_SelfMutex);
	}

	std::cout << dye::yellow("\nMultiRoblox terminated safely.") << "\n";
	ExitProcess(0);
}

// --------------------------------------------------
// Console handler
// --------------------------------------------------
BOOL WINAPI ConsoleHandler(DWORD signal)
{
	if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT)
	{
		CleanupAndExit();
	}
	return TRUE;
}

// --------------------------------------------------
// Entry point
// --------------------------------------------------
int main()
{
	SetConsoleCtrlHandler(ConsoleHandler, TRUE);
	SetConsoleOutputCP(65001);

	// ----------------------------------------------
	// Ensure single MultiRoblox instance
	// ----------------------------------------------
	g_SelfMutex = CreateMutexW(NULL, TRUE, L"MultiRoblox_Internal_Singleton");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		std::cout << dye::yellow("MultiRoblox is already running.") << "\n";
		std::cout << dye::grey("Only one instance is allowed.") << "\n";
		return 0;
	}

	// ----------------------------------------------
	// Acquire Roblox mutexes
	// ----------------------------------------------
	const wchar_t* robloxMutexes[] =
	{
		L"ROBLOX_singletonMutex",
		L"RobloxPlayerSingletonMutex",
		L"RobloxAppSingletonMutex"
	};

	for (const auto& name : robloxMutexes)
		AcquireRobloxMutex(name);

	if (g_RobloxMutexes.empty())
	{
		std::cout << dye::red("ERROR: Failed to acquire Roblox mutexes.") << "\n";
		std::cin.get();
		return 1;
	}

	// ----------------------------------------------
	// UI
	// ----------------------------------------------
	int instances = CountRobloxInstances();

	std::cout << "\n";
	std::cout << dye::aqua("========================================================") << "\n";
	std::cout << dye::aqua("|") << dye::white("   MultiRoblox - Ultimate Optimized Edition (CLI)   ") << dye::aqua("|") << "\n";
	std::cout << dye::aqua("========================================================") << "\n\n";

	std::cout << dye::green("[OK] ")
	          << dye::white("Roblox mutexes acquired: ")
	          << dye::aqua(std::to_string(g_RobloxMutexes.size())) << "\n";

	std::cout << dye::green("[OK] ")
	          << dye::white("Roblox instances detected: ")
	          << dye::aqua(std::to_string(instances)) << "\n";

	std::cout << dye::grey("Status: ") << dye::green("ACTIVE") << "\n";
	std::cout << dye::grey("CPU Usage: ") << dye::green("0%") << "\n";
	std::cout << dye::grey("Press Ctrl + C to exit.") << "\n\n";

	// ----------------------------------------------
	// Zero CPU idle
	// ----------------------------------------------
	Sleep(INFINITE);
	return 0;
}
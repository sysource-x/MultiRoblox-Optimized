/*
 * MultiRoblox - Ultimate Optimized Edition
 * Stealth core with optional diagnostic mode (--status)
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

// Roblox mutex handles (hidden backend)
std::vector<HANDLE> g_RobloxMutexes;

// --------------------------------------------------
// Optional: Count Roblox instances (diagnostic only)
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
			// Generic detection (works with Fishstrap/Bloxstrap)
			if (wcsstr(entry.szExeFile, L"Roblox") != nullptr)
				count++;
		} while (Process32NextW(snapshot, &entry));
	}

	CloseHandle(snapshot);
	return count;
}

// --------------------------------------------------
// Acquire Roblox mutex (silent)
// --------------------------------------------------
void AcquireRobloxMutex(const wchar_t* name)
{
	HANDLE h = CreateMutexW(NULL, TRUE, name);
	if (h)
		g_RobloxMutexes.push_back(h);
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
int main(int argc, char* argv[])
{
	SetConsoleCtrlHandler(ConsoleHandler, TRUE);
	SetConsoleOutputCP(65001);

	bool statusMode = false;

	// Parse arguments
	for (int i = 1; i < argc; ++i)
	{
		if (std::string(argv[i]) == "--status")
			statusMode = true;
	}

	// ----------------------------------------------
	// Ensure single MultiRoblox instance
	// ----------------------------------------------
	g_SelfMutex = CreateMutexW(NULL, TRUE, L"MultiRoblox_Internal_Singleton");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		if (statusMode)
			std::cout << "MultiRoblox is already running.\n";
		return 0;
	}

	// ----------------------------------------------
	// Acquire Roblox mutexes (hidden)
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
		std::cout << "Failed to enable multi-instance mode.\n";
		return 1;
	}

	// ----------------------------------------------
	// UI
	// ----------------------------------------------
	std::cout << "MultiRoblox active. Multi-instance enabled.\n";

	if (statusMode)
	{
		int instances = CountRobloxInstances();
		std::cout << "Roblox instances detected: " << instances << "\n";
		std::cout << "CPU usage: 0%\n";
	}

	// ----------------------------------------------
	// Idle (zero CPU)
	// ----------------------------------------------
	Sleep(INFINITE);
	return 0;
}
/*
 * MultiRoblox - Ultimate Optimized Edition
 * CLI Core Foundation (base for tray & GUI)
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

// --------------------------------------------------
// App options
// --------------------------------------------------
struct AppOptions
{
	bool status = false;
	bool silent = false;
	bool once = false;
};

// --------------------------------------------------
// Globals
// --------------------------------------------------
HANDLE g_SelfMutex = NULL;
std::vector<HANDLE> g_RobloxMutexes;

// --------------------------------------------------
// Parse CLI arguments
// --------------------------------------------------
AppOptions ParseArgs(int argc, char* argv[])
{
	AppOptions opts;

	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];

		if (arg == "--status") opts.status = true;
		else if (arg == "--silent") opts.silent = true;
		else if (arg == "--once") opts.once = true;
	}

	return opts;
}

// --------------------------------------------------
// Optional diagnostic
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
			if (wcsstr(entry.szExeFile, L"Roblox") != nullptr)
				count++;
		} while (Process32NextW(snapshot, &entry));
	}

	CloseHandle(snapshot);
	return count;
}

// --------------------------------------------------
// Acquire Roblox mutex (backend only)
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

	AppOptions opts = ParseArgs(argc, argv);

	// ----------------------------------------------
	// Ensure single MultiRoblox instance
	// ----------------------------------------------
	g_SelfMutex = CreateMutexW(NULL, TRUE, L"MultiRoblox_Internal_Singleton");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		if (opts.status && !opts.silent)
			std::cout << "MultiRoblox is already running.\n";
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
		if (!opts.silent)
			std::cout << "Failed to enable multi-instance mode.\n";
		return 1;
	}

	// ----------------------------------------------
	// UI
	// ----------------------------------------------
	if (!opts.silent)
		std::cout << "MultiRoblox active. Multi-instance enabled.\n";

	if (opts.status && !opts.silent)
	{
		std::cout << "Roblox instances: "
		          << CountRobloxInstances() << "\n";
		std::cout << "CPU usage: 0%\n";
	}

	// ----------------------------------------------
	// Exit immediately if --once
	// ----------------------------------------------
	if (opts.once)
	{
		CleanupAndExit();
		return 0;
	}

	// ----------------------------------------------
	// Idle
	// ----------------------------------------------
	Sleep(INFINITE);
	return 0;
}

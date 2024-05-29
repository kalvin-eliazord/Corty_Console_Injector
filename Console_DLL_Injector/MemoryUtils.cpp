#include "MemoryUtils.h"

namespace fs = std::filesystem;

std::vector<PROCESSENTRY32> MemoryUtils::GetProcList()
{
	std::vector<PROCESSENTRY32> procList{};

	HANDLE hSnap{ CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL) };

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				procList.push_back(procEntry);
			} while (Process32Next(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);
	return procList;
}

void MemoryUtils::SetDLLName()
{
	std::string currentDir{ "./" };

	bool bDLLNotFound{ true };

	for (const auto& file : fs::directory_iterator(currentDir))
	{
		if (file.path().extension() == ".dll")
		{
			dllName = file.path().filename().generic_string();
			bDLLNotFound = false;
			break;
		}
	}

	// making name empty if no file found
	if (bDLLNotFound) dllName.clear();
}

MemoryUtils::MemoryUtils()
{
	SetDLLName();
}

std::string_view MemoryUtils::GetDLLName()
{
	return dllName;
}

std::string MemoryUtils::GetDLLCurrDirectory()
{
	const int bufferSize{ 1024 };
	char bufferDir[bufferSize];

	if (_getcwd(bufferDir, bufferSize))
		return bufferDir;

	return "[!] Can't get current directory path.";
}

bool MemoryUtils::WinAPI_Injection(DWORD pProcId)
{
	// Set DLL path
	std::string dllCurrDir{ GetDLLCurrDirectory() };

	std::string dllPath{ dllCurrDir + std::string("\\" + dllName) };

	HANDLE hProc{ OpenProcess(PROCESS_ALL_ACCESS, 0, pProcId) };

	if (hProc == INVALID_HANDLE_VALUE)
	{
		std::cerr << "[!] Failed to open process. Error: \n" << GetLastError();;
		return false;
	}

	// Allocate new mem
	void* memAlloc{ VirtualAllocEx(hProc, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE) };

	if (!memAlloc)
	{
		std::cerr << "[!] Failed to allocate memory. Error: \n" << GetLastError();
		CloseHandle(hProc);
		return false;
	}

	//std::cout << "[+] dllPath: \n" << dllPath << '\n';

	SIZE_T bytesWritten;
	if (!WriteProcessMemory(hProc, memAlloc, dllPath.c_str(), strlen(dllPath.c_str()) + 1, &bytesWritten)
		|| !bytesWritten)
	{
		std::cerr << "[!] Failed to write process memory. Error: \n" << GetLastError();
		VirtualFreeEx(hProc, memAlloc, 0, MEM_RELEASE);
		CloseHandle(hProc);
		return false;
	}
	
	HANDLE remoteThread{ CreateRemoteThread(hProc, nullptr, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, memAlloc,  NULL, nullptr) };

	if (!remoteThread || remoteThread == INVALID_HANDLE_VALUE)
	{
		std::cerr << "[!] Failed to create remote thread. Error:\n" << GetLastError();;
		VirtualFreeEx(hProc, memAlloc, 0, MEM_RELEASE);
		CloseHandle(hProc);
		return false;
	}

	WaitForSingleObject(remoteThread, INFINITE);
	CloseHandle(remoteThread);

	VirtualFreeEx(hProc, memAlloc, 0, MEM_RELEASE);
	CloseHandle(hProc);

	return true;
}

bool MemoryUtils::ManualMapping_Injection(DWORD pProcId)
{
	// TODO
	return true;
}

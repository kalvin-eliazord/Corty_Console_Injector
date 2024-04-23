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

void MemoryUtils::SetDllName()
{
    std::string currentDir{ "./" };

    for (const auto& file : fs::directory_iterator(currentDir))
    {
        if (file.path().extension() == ".dll")
        {
            dllName = file.path().filename().generic_wstring();
            break;
        }
    }
}

std::wstring MemoryUtils::GetDllCurrDirectory()
{
    wchar_t pathDirBuffer[MAX_PATH];

    if (!_wgetcwd(pathDirBuffer, MAX_PATH))
        return L"Can't get current directory path.";

    std::wstring currWorkingDir(pathDirBuffer);

    return currWorkingDir;
}

void MemoryUtils::InjectDllIntoProc(DWORD pProcId)
{
    // SetDllName
    SetDllName();

    // SetDll path
    std::wstring dllPath{ GetDllCurrDirectory()};

    std::wstring dllComplete{ dllPath + std::wstring(L"\\" + dllName) };

    HANDLE hProc{ OpenProcess(PROCESS_ALL_ACCESS, 0, pProcId)};

    if (hProc != INVALID_HANDLE_VALUE)
    {
        // allocate new mem
        uintptr_t* memAlloc{ (uintptr_t*) VirtualAllocEx(hProc, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE) };

        std::wcout << "here: " << dllComplete << "\n";

        // writing dll Path into process
        if (memAlloc)
            WriteProcessMemory(hProc, memAlloc, &dllComplete, dllComplete.length()+ 1, nullptr);

        // create remote thread to load dll into the process selected 
        HANDLE remoteThread{ CreateRemoteThread(hProc, nullptr, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, memAlloc,  NULL, nullptr) };

        // closing handles
        CloseHandle(hProc);

        if(remoteThread)
            CloseHandle(remoteThread);
    }

}
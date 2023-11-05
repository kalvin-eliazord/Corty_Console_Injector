#include "header.h"

namespace fs = std::filesystem;

std::vector<PROCESSENTRY32> MemManagement::GetProcList()
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

std::wstring MemManagement::GetDLLName()
{
    std::string currentDir{ "./" };
    std::wstring dllName{ L" There is no DLL in the folder! Please insert .dll file then [F5]." };

    int i{ 0 };
    for (const auto& entry : fs::directory_iterator(currentDir))
    {
        if (entry.path().extension() == ".dll")
            dllName = entry.path().filename().generic_wstring();
    }

    return dllName;
}

void MemManagement::InjectDllInto(PROCESSENTRY32 pProcess)
{
    // setting dllPath
    const wchar_t* dllName{ pProcess.szExeFile};
    const char*    dllDir{ "./"};

    HANDLE hProc{ OpenProcess(PROCESS_ALL_ACCESS, 0, pProcess.th32ProcessID)};

    if (hProc && hProc != INVALID_HANDLE_VALUE)
    {
        // allocate new mem
        uintptr_t* memAlloc{ (uintptr_t*) VirtualAllocEx(hProc, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE) };

        // writing dll Path into process
        if (memAlloc)
        {
            WriteProcessMemory(hProc, memAlloc, &dllDir, strlen(dllDir), nullptr);
            WriteProcessMemory(hProc, memAlloc+2, &dllName, wcslen(dllName)+1, nullptr);
        }

        // create remote thread to load dll into the process selected
        HANDLE remoteThread{ CreateRemoteThread(hProc, nullptr, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, memAlloc,  NULL, nullptr) };

        // closing handles
        CloseHandle(hProc);

        if(remoteThread)
            CloseHandle(remoteThread);
    }

}

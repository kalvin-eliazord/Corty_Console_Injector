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

std::wstring MemManagement::GetDllName()
{
    std::string currentDir{ "./" };
    std::wstring dllName{ L" There is no DLL in the folder! Please insert .dll file." };

    int i{ 0 };
    for (const auto& entry : fs::directory_iterator(currentDir))
    {
        if (entry.path().extension() == ".dll")
            dllName = entry.path().filename().generic_wstring();
    }

    return dllName;
}

std::wstring MemManagement::GetDllCurrDirectory()
{
    wchar_t buff[MAX_PATH];

    _wgetcwd(buff, MAX_PATH);

    std::wstring current_working_dir(buff);

    return current_working_dir;
}

void MemManagement::InjectDllInto(PROCESSENTRY32 pProcess)
{
    // setting dllPath
    std::wstring dllName{ GetDllName()};
    std::wstring dllPath{ GetDllCurrDirectory()};

    std::wstring dllComplete{ dllPath + std::wstring(L"\\" + dllName) };

    std::wcout << "here: " << dllComplete << "\n";
    HANDLE hProc{ OpenProcess(PROCESS_ALL_ACCESS, 0, pProcess.th32ProcessID)};

    if (hProc && hProc != INVALID_HANDLE_VALUE)
    {
        // allocate new mem
        uintptr_t* memAlloc{ (uintptr_t*) VirtualAllocEx(hProc, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE) };

        // writing dll Path into process
        if (memAlloc)
            WriteProcessMemory(hProc, memAlloc, &dllPath, dllPath.length()+ 1, nullptr);

        // create remote thread to load dll into the process selected
        HANDLE remoteThread{ CreateRemoteThread(hProc, nullptr, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, memAlloc,  NULL, nullptr) };

        // closing handles
        CloseHandle(hProc);

        if(remoteThread)
            CloseHandle(remoteThread);
    }

}
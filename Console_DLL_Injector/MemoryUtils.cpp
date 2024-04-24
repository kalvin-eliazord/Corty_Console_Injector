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

    bool bDllNotFound{ true };

    for (const auto& file : fs::directory_iterator(currentDir))
    {
        if (file.path().extension() == ".dll")
        {
            dllName = file.path().filename().generic_string();
            bDllNotFound = false;
            break;
        }
    }

    // making name empty if no file found
    if(bDllNotFound) dllName.clear();
}

MemoryUtils::MemoryUtils()
{
    SetDllName();
}

std::string_view MemoryUtils::GetDllName()
{
    return dllName;
}

std::string MemoryUtils::GetDllCurrDirectory()
{
    const int bufferSize{ 1024 };
    char bufferDir[bufferSize];

    if (_getcwd(bufferDir, bufferSize))
        return bufferDir;

    return "Can't get current directory path.";
}

bool MemoryUtils::InjectDllIntoProc(DWORD pProcId)
{
    // Set Dll path
    std::string dllCurrDir{ GetDllCurrDirectory()};

    std::string dllPath{ dllCurrDir + std::string("\\" + dllName) };

    HANDLE hProc{ OpenProcess(PROCESS_ALL_ACCESS, 0, pProcId)};

    if (hProc != INVALID_HANDLE_VALUE)
    {
        // Allocate new mem
        void* memAlloc{ VirtualAllocEx(hProc, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE) };

        std::cout << "dllPath: \n" << dllPath << "\n";

        // Writing Dll path into process selected
        if (!memAlloc) return false;
        WriteProcessMemory(hProc, memAlloc, dllPath.c_str(), strlen(dllPath.c_str())+ 1, nullptr);

        // create remote thread to load dll into the process selected 
        HANDLE remoteThread{ CreateRemoteThread(hProc, nullptr, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, memAlloc,  NULL, nullptr) };

        if (remoteThread) CloseHandle(remoteThread);

        CloseHandle(hProc);

        return true;
    }

    return false;
}
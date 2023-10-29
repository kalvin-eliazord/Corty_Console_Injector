#include "header.h"

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

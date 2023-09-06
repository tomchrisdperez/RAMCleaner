#include "stdafx.h"
#include "MemoryUtil.h"

#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"psapi.lib")

void AdjustTokenPrivilegesForNT() {
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    OpenProcessToken(GetCurrentProcess(),
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
    LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
}

Info EmptyAllSet() {
    HANDLE SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (SnapShot == NULL) return { FALSE, 0, 0, 0 };
    PROCESSENTRY32 ProcessInfo;
    ProcessInfo.dwSize = sizeof(ProcessInfo);
    int count = 0;
    BOOL Status = Process32First(SnapShot, &ProcessInfo);
    while (Status) {
        count++;
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE,
            ProcessInfo.th32ProcessID);
        if (hProcess) {
            SetProcessWorkingSetSize(hProcess, -1, -1);
            EmptyWorkingSet(hProcess);
            CloseHandle(hProcess);
        }
        Status = Process32Next(SnapShot, &ProcessInfo);
    }
    MEMORYSTATUSEX memStatus;
    typedef  void(WINAPI* FunctionGlobalMemoryStatusEx)(LPMEMORYSTATUSEX);
    memStatus.dwLength = sizeof(memStatus);
    FunctionGlobalMemoryStatusEx MyGlobalMemoryStatusEx;
    MyGlobalMemoryStatusEx = (FunctionGlobalMemoryStatusEx)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "GlobalMemoryStatusEx");
    if (NULL == MyGlobalMemoryStatusEx) return{ FALSE, 0, 0, 0 };
    MyGlobalMemoryStatusEx(&memStatus);
    return { TRUE, count, (int)(memStatus.ullTotalPhys / 1024 / 1024), (int)(memStatus.ullAvailPhys / 1024 / 1024) };
}
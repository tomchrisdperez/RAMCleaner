#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

struct Info {
    BOOL isOk;
    int processess;
    int TotalMem;
    int AvaiMem;
};

void AdjustTokenPrivilegesForNT();
Info EmptyAllSet();
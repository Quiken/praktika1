#include <windows.h>
#include <stdio.h>
#include <psapi.h>
#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "netapi32.lib")

#include <stdio.h>
#include <assert.h>
#include <windows.h> 
#include <lm.h>
#include <intrin.h>
#include "tchar.h"
#include <iostream>
using namespace std;

void PrintMemoryInfo(DWORD processID)
{
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS pmc;
    printf("\nProcess ID: %u\n", processID);
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
        PROCESS_VM_READ,
        FALSE, processID);
    if (NULL == hProcess)
        return;

    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
    {
        printf("\tPageFaultCount: 0x%08X\n", pmc.PageFaultCount);
        printf("\tPeakWorkingSetSize: 0x%08X\n",
            pmc.PeakWorkingSetSize);
        printf("\tWorkingSetSize: 0x%08X\n", pmc.WorkingSetSize);
        printf("\tQuotaPeakPagedPoolUsage: 0x%08X\n",
            pmc.QuotaPeakPagedPoolUsage);
        printf("\tQuotaPagedPoolUsage: 0x%08X\n",
            pmc.QuotaPagedPoolUsage);
        printf("\tQuotaPeakNonPagedPoolUsage: 0x%08X\n",
            pmc.QuotaPeakNonPagedPoolUsage);
        printf("\tQuotaNonPagedPoolUsage: 0x%08X\n",
            pmc.QuotaNonPagedPoolUsage);
        printf("\tPagefileUsage: 0x%08X\n", pmc.PagefileUsage);
        printf("\tPeakPagefileUsage: 0x%08X\n",
            pmc.PeakPagefileUsage);
    }
    CloseHandle(hProcess);
}

void CPU_info(int argc, _TCHAR* argv[]) {
    
}

int _tmain(int argc, _TCHAR* argv[])
{
    // Get the list of process identifiers.
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        return 1;
    }
    cProcesses = cbNeeded / sizeof(DWORD);

    for (i = 0; i < cProcesses; i++)
    {
        PrintMemoryInfo(aProcesses[i]);
    }

    /// <summary>
    /// ПОЛЬЗОВАТЕЛЬ
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    /// 
    /// 

    LPWKSTA_USER_INFO_0 pBuf = NULL;
    LPWKSTA_USER_INFO_0 pTmpBuf;
    DWORD dwLevel = 0;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
    DWORD j;
    DWORD dwTotalCount = 0;
    NET_API_STATUS nStatus;
    LPWSTR pszServerName = NULL;

    if (argc > 2)
    {
        fwprintf(stderr, L"Usage: %s [\\\\ServerName]\n", argv[0]);
        exit(1);
    }
    // The server is not the default local computer.
    //
    if (argc == 2)
        pszServerName = argv[1];
    fwprintf(stderr, L"\nUsers currently logged on %s:\n", pszServerName);
   
    do // begin do
    {
        nStatus = NetWkstaUserEnum(pszServerName,
            dwLevel,
            (LPBYTE*)&pBuf,
            dwPrefMaxLen,
            &dwEntriesRead,
            &dwTotalEntries,
            &dwResumeHandle);
      
        if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
        {
            if ((pTmpBuf = pBuf) != NULL)
            {
               
                for (j = 0; (j < dwEntriesRead); j++)
                {
                    assert(pTmpBuf != NULL);

                    if (pTmpBuf == NULL)
                    {
                        
                        fprintf(stderr, "An access violation has occurred\n");
                        break;
                    }
                 
                    wprintf(L"\t-- %s\n", pTmpBuf->wkui0_username);

                    pTmpBuf++;
                    dwTotalCount++;
                }
            }
        }
      
        else
            fprintf(stderr, "A system error has occurred: %d\n", nStatus);
        //
        // Free the allocated memory.
        //
        if (pBuf != NULL)
        {
            NetApiBufferFree(pBuf);
            pBuf = NULL;
        }
    }
  
    while (nStatus == ERROR_MORE_DATA); 
   
    if (pBuf != NULL)
        NetApiBufferFree(pBuf);
 
    fprintf(stderr, "\nTotal of %d entries enumerated\n", dwTotalCount);

    // Get extended ids.
    int CPUInfo[4] = { -1 };
    __cpuid(CPUInfo, 0x80000000);
    unsigned int nExIds = CPUInfo[0];

    // Get the information associated with each extended ID.
    char CPUBrandString[0x40] = { 0 };
    for (unsigned int i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuid(CPUInfo, i);

        // Interpret CPU brand string and cache information.
        if (i == 0x80000002)
        {
            memcpy(CPUBrandString,
                CPUInfo,
                sizeof(CPUInfo));
        }
        else if (i == 0x80000003)
        {
            memcpy(CPUBrandString + 16,
                CPUInfo,
                sizeof(CPUInfo));
        }
        else if (i == 0x80000004)
        {
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
        }
    }
    cout << "Cpu String:" << CPUBrandString;
    system("pause");

    return 0;
}

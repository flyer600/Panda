// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <stdio.h>

void MsgBox(const char* szMsg) {
    char szModuleName[MAX_PATH] = {0}; 
    GetModuleFileName(NULL, szModuleName, MAX_PATH);
    MessageBox(NULL, szMsg, szModuleName, MB_OK);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        MsgBox("Porcess Attach Inject into process!");
    case DLL_THREAD_ATTACH:
        printf("Thread Attach Inject into process!\n");
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


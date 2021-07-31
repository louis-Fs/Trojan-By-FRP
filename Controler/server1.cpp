#include <iostream>
#include <winsock2.h>
#include <process.h>
#include <Windows.h>
#include <string>
#include "server.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;

#define PORT 65432

unsigned __stdcall FunProc(LPVOID lpParam);
BOOL InitWinsock();
BOOL FileReceive(SOCKET Socket_, const char *name, int seq);
int file_size(const char *filename);
void ShowMenu(void);
int GetDir(char **dir);
int SaveBitmapToFile(HBITMAP hBitmap, LPSTR lpFileName);
void BmpCreate(char *name);
BOOL FileDirList(SOCKET Socket_, int seq);

char bmpname[512] = {0};

int main(int argc, char *argv[])
{
	//��ʾ�ͻ��˵�ַ
	struct client_info *clientinfo;
	struct sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	//��ʼ��
	InitWinsock();
	//�����׽���
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		cout << "create socket error !" << endl;
		return 0;
	}
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	//��IP�Ͷ˿�
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		cout << "bind error !" << endl;
	}
	//��ʼ����
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		cout << "listen error !" << endl;
		return 0;
	}
	//ѭ�� ���� �����ͻ��˵�����
	//char revData[255];
	cout << "server is ok!" << endl;
	while (true)
	{
		SOCKET sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET)
		{
			cout << "accept error !" << endl;
			continue;
		}
		cout << "���ܵ�һ�����ӣ�" << inet_ntoa(remoteAddr.sin_addr) << endl;
		clientinfo = (struct client_info *)malloc(sizeof(struct client_info));
		clientinfo->client_address.sin_addr = remoteAddr.sin_addr;
		clientinfo->client = sClient;
		command mycommand;
		const char *sendData = "Hello From myServer��\n";
		ZeroMemory(&mycommand, sizeof(mycommand));
		mycommand.seq = 0;
		strncpy(mycommand.bank, sendData, strlen(sendData));
		send(sClient, (char *)&mycommand, sizeof(command), 0);
		printf("%d seq %s data\n", mycommand.seq, mycommand.bank);
		unsigned int threadid;
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, FunProc, (LPVOID *)clientinfo, 0, &threadid);
		CloseHandle(hThread);
	}
	closesocket(slisten);
	WSACleanup();
	return 0;
}

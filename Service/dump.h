#ifndef __DUMP_H__
#define __DUMP_H__
//*************************************************************************************
//  �ļ��� : find.c
//  ���� : one_process_find
//  ���� : louis   �޸��� : louis   ����Ż�ע���� : louis
//  ���˲��� : 
//  csdn code: 
//  ���� : �����ض�����
//  ���뻷�� : Windows XP SP3 + vc6.0
//  ������� : 2020��11��15�� 
//*************************************************************************************


#include <windows.h>
#include "tlhelp32.h"

int GetDir_(char **dir);
BOOL LoadRemoteDLL(DWORD dwProcessId, LPTSTR lpszLibPath);
BOOL GetProcessIdByName(LPSTR szProcessName, LPDWORD lpPID);
int EnableDebugPriv(const char *name);
int dll(char *DllPath, char *ProcessName);


    //����˵����ʼ
    //*********************************************************************************
    //  ���� : ��ȡ���̵ĵ���Ȩ��
    //  ���� : const char *name
    //  (���)  name : ָ��Ȩ������,���������õ�SE_DEBUG_NAME
    //    #define          SE_BACKUP_NAME           TEXT("SeBackupPrivilege")
    //    #define          SE_RESTORE_NAME          TEXT("SeRestorePrivilege")
    //    #define          SE_SHUTDOWN_NAME         TEXT("SeShutdownPrivilege")
    //    #define          SE_DEBUG_NAME            TEXT("SeDebugPrivilege")
    //  ���� : -1��ʾ��ȡȨ��ʧ��, 0��ʾ��ȡȨ�޳ɹ�
    //  ��Ҫ˼· : �ȴ򿪽������ƻ�,Ȼ���ñ��ؽ���name�������Ȩ�����͵ľֲ�ΨһID
    //             ����������Ȩ��
    //  ���þ��� : EnableDebugPriv(SE_DEBUG_NAME)
    //  ���� : 2020��11��15�� 19:08:22(ע������)
    //**********************************************************************************
    //����˵������
int EnableDebugPriv(const char *name)
{
    HANDLE hToken;        //�������ƾ��
    TOKEN_PRIVILEGES tp;  //TOKEN_PRIVILEGES�ṹ�壬���а���һ��������+��������Ȩ������
    LUID luid;           //�����ṹ���е�����ֵ

    //�򿪽������ƻ�
    //GetCurrentProcess()��ȡ��ǰ���̵�α�����ֻ��ָ��ǰ���̻����߳̾������ʱ�仯
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken))
    {
       fprintf(stderr,"OpenProcessToken error\n");
       return -1;
    }

    //��ñ��ؽ���name�������Ȩ�����͵ľֲ�ΨһID
    if (!LookupPrivilegeValue(NULL, name, &luid))
    {
       fprintf(stderr,"LookupPrivilegeValue error\n");
    }

    tp.PrivilegeCount = 1;                               //Ȩ��������ֻ��һ����Ԫ�ء�
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;  //Ȩ�޲���
    tp.Privileges[0].Luid = luid;                        //Ȩ������

    //��������Ȩ��
    if (!AdjustTokenPrivileges(hToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
    {
       fprintf(stderr,"AdjustTokenPrivileges error!\n");
       return -1;
    }

    return 0;

}
	//����˵����ʼ
    //**********************************************************************************
    //  ���� : ����ĳһ�ض����̲���ȡPID
    //  ���� : LPSTR szProcessName,LPDWORD lpPID
    //  
    //  LPSTR =char* ; LPDWORD= unsigned long
    //    
    //  ���� : return true says that success,false says failed
    //  ��Ҫ˼· : 
    //  
    //  
    //  ���� : 2020��11��15�� (ע������)
    //**********************************************************************************
    //����˵������

BOOL GetProcessIdByName(LPSTR szProcessName, LPDWORD lpPID)
{
	PROCESSENTRY32 ps;
	HANDLE hSnapshot;
	BOOL bProcess;

	//���ս�����Ϣ��һ���ṹ�壬�����ڴ�ռ�
	ZeroMemory(&ps,sizeof(PROCESSENTRY32));
	ps.dwSize = sizeof(PROCESSENTRY32);

	//�����������Ŀ���
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hSnapshot == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	//��ȡ��һ�����̵���Ϣ
	bProcess=Process32First(hSnapshot,&ps);
	while(bProcess)
	{
		if(lstrcmpi(ps.szExeFile,szProcessName)==0)
		{
			*lpPID= ps.th32ProcessID;
			CloseHandle(hSnapshot);
			return TRUE;
		}
		//ѭ��������һ������
		bProcess=Process32Next(hSnapshot,&ps);
	}
	CloseHandle(hSnapshot);

	return FALSE;
}


	//����˵����ʼ
    //**********************************************************************************
    //  ���� : ��Զ�̽��̼���DLL��DLLע�룩
    //  ���� : DWORD dwProcessId Ŀ�����ID
	//         LPTSTR lpszLibPath 
    //  ���� : PDWORD ���ڴ��ַ
    //  ��Ҫ˼· : 
    //  
    //  
    //  ���� : 2020��11��15�� (ע������)
    //**********************************************************************************
    //����˵������

BOOL LoadRemoteDLL(DWORD dwProcessId, LPTSTR lpszLibPath)
{
	BOOL bReasult = FALSE;
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	PSTR psszLibFileRemote = NULL;
	DWORD PathLength;
	PTHREAD_START_ROUTINE pfnThreadRtn;
	__try
	{
		//���Ŀ����̵ľ��
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
		if(hProcess == NULL)
		{
			printf("���Ŀ����̾��ʧ��\n");
			__leave;
		}

		//��ü���DLL·���ĳ���
		PathLength = 1 + lstrlen(lpszLibPath);
		printf("PathLength:%d ******\n",PathLength);
		printf("%s",lpszLibPath);
		
		//��Զ�̽�����ΪDLL·�������ڴ�ռ�
		psszLibFileRemote = (PSTR)VirtualAllocEx(hProcess,NULL,PathLength,MEM_COMMIT,PAGE_READWRITE);
		if(psszLibFileRemote == NULL)
		{
			printf("ΪDLL·�������ڴ�ռ�ʧ��\n");
			__leave;
		}

		//��DLL��·�������Ƶ�Զ�̽��̵��ڴ�ռ�
		if(!WriteProcessMemory(hProcess,(LPVOID)psszLibFileRemote,(LPVOID)lpszLibPath,PathLength,NULL))
		{
			printf("��DLL��·�������Ƶ�Զ�̽��̵��ڴ�ռ����\n");
			__leave;
		}

		//���LoadLibraryA��Kernel32�е�������ַ
		pfnThreadRtn = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), TEXT("LoadLibraryA"));
		if(pfnThreadRtn ==NULL )
		{
			printf("���LoadLibraryA��Kernel32�е�������ַʧ��\n");
			__leave;
		}

		//����Զ���̲߳�ͨ��Զ���̵߳���DLL�ļ�
		hThread = CreateRemoteThread(hProcess,NULL,0,pfnThreadRtn,(LPVOID)psszLibFileRemote,0,NULL);
		if(hThread == NULL)
		{
			printf("Զ���̴߳���ʧ��\n");
			__leave;
		}
		
		//�ȴ�Զ���߳���ֹ
		WaitForSingleObject(hThread,INFINITE);
		bReasult = TRUE;

	}
	__finally
	{
		//�رվ��
		if(psszLibFileRemote != NULL)
		{
			VirtualFreeEx(hProcess,(LPVOID)psszLibFileRemote,0,MEM_RELEASE);
		}
		if(hThread != NULL)
		{
			CloseHandle(hThread);
		}
		if(hProcess != NULL)
		{
			CloseHandle(hProcess);
		}
	}

	return bReasult;

}
int GetDir_(char **dir)
//BOOL GetDir(void )
{
	char szNowDirPath[MAX_PATH];
	DWORD dwNowDirLength;
	unsigned int temp_length;
	char SubDir[MAX_PATH]={0};

	int count = 0;
	char split = '\\';
	int zero = 0;
	dwNowDirLength=GetCurrentDirectory(MAX_PATH,szNowDirPath);
	temp_length=dwNowDirLength;
	if(dwNowDirLength==0)
	{
		printf("��ǰĿ¼��ȡʧ��\n");
		return 0;
	}
	for(;(int)dwNowDirLength>0;dwNowDirLength--)
	{
		if(szNowDirPath[temp_length-dwNowDirLength]==0x5c)
		{
			SubDir[count]=szNowDirPath[temp_length-dwNowDirLength];
			count++;
			SubDir[count]=split;
			count++;
		}else
		{
			SubDir[count]=szNowDirPath[temp_length-dwNowDirLength];
			count++;
		}

	}
	SubDir[count]=split;
	SubDir[count+1]=split;
	printf("%d\n",count);
	printf("%d\n",temp_length);

	printf("++++++++++++++%s+++++++++++++\n",SubDir);
	*dir=SubDir;
	return 1;
}
int dll(char *DllPath, char *ProcessName)
{
	unsigned long id=0;
	//char CurrentDir[MAX_PATH]={0};
	//char *Dir;
	//char *DllName="C:\\Users\\louis\\Desktop\\dump\\evil_dll.dll";
/*
	GetDir(&Dir);
	strncat(CurrentDir,Dir,strlen(Dir));
	strncat(CurrentDir,DllName,strlen(DllName));
	printf("DLL is located at****%s****\n",CurrentDir);
	*/
    if(EnableDebugPriv(SE_DEBUG_NAME))      //��ȡԶ�̽��̵���Ȩ��
    {
        fprintf(stderr, "Add Privilege Failed!!\n");
		return 0;
    }
	//��ȡĿ����̵�PID
	GetProcessIdByName(ProcessName,&id);
	printf("%d\n",id);
	if(id<1)
	{
		printf("��ȡIDʧ��\n");
		return 0;
	}
	
	//Զ�̼���DLL
	//if(LoadRemoteDLL(id,"C:\\Documents and Settings\\root\\����\\VC6\\one_process_find\\evil_dll.dll"))
	if(LoadRemoteDLL(id,DllPath))
	{
		printf("Զ�̽���DLLע��ɹ�\n");
	}else
	{
		printf("Զ��DLLע��ʧ��\n");
		return 0;
	}
	return 1;
}
#endif
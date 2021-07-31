#ifndef __DUMP_H__
#define __DUMP_H__
//*************************************************************************************
//  文件名 : find.c
//  工程 : one_process_find
//  作者 : louis   修改者 : louis   最后优化注释者 : louis
//  个人博客 : 
//  csdn code: 
//  描述 : 查找特定进程
//  编译环境 : Windows XP SP3 + vc6.0
//  完成日期 : 2020年11月15日 
//*************************************************************************************


#include <windows.h>
#include "tlhelp32.h"

int GetDir_(char **dir);
BOOL LoadRemoteDLL(DWORD dwProcessId, LPTSTR lpszLibPath);
BOOL GetProcessIdByName(LPSTR szProcessName, LPDWORD lpPID);
int EnableDebugPriv(const char *name);
int dll(char *DllPath, char *ProcessName);


    //函数说明开始
    //*********************************************************************************
    //  功能 : 获取进程的调试权限
    //  参数 : const char *name
    //  (入口)  name : 指向权限名称,我们这里用到SE_DEBUG_NAME
    //    #define          SE_BACKUP_NAME           TEXT("SeBackupPrivilege")
    //    #define          SE_RESTORE_NAME          TEXT("SeRestorePrivilege")
    //    #define          SE_SHUTDOWN_NAME         TEXT("SeShutdownPrivilege")
    //    #define          SE_DEBUG_NAME            TEXT("SeDebugPrivilege")
    //  返回 : -1表示获取权限失败, 0表示获取权限成功
    //  主要思路 : 先打开进程令牌环,然后获得本地进程name所代表的权限类型的局部唯一ID
    //             最后调整进程权限
    //  调用举例 : EnableDebugPriv(SE_DEBUG_NAME)
    //  日期 : 2020年11月15日 19:08:22(注释日期)
    //**********************************************************************************
    //函数说明结束
int EnableDebugPriv(const char *name)
{
    HANDLE hToken;        //进程令牌句柄
    TOKEN_PRIVILEGES tp;  //TOKEN_PRIVILEGES结构体，其中包含一个【类型+操作】的权限数组
    LUID luid;           //上述结构体中的类型值

    //打开进程令牌环
    //GetCurrentProcess()获取当前进程的伪句柄，只会指向当前进程或者线程句柄，随时变化
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken))
    {
       fprintf(stderr,"OpenProcessToken error\n");
       return -1;
    }

    //获得本地进程name所代表的权限类型的局部唯一ID
    if (!LookupPrivilegeValue(NULL, name, &luid))
    {
       fprintf(stderr,"LookupPrivilegeValue error\n");
    }

    tp.PrivilegeCount = 1;                               //权限数组中只有一个“元素”
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;  //权限操作
    tp.Privileges[0].Luid = luid;                        //权限类型

    //调整进程权限
    if (!AdjustTokenPrivileges(hToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
    {
       fprintf(stderr,"AdjustTokenPrivileges error!\n");
       return -1;
    }

    return 0;

}
	//函数说明开始
    //**********************************************************************************
    //  功能 : 查找某一特定进程并获取PID
    //  参数 : LPSTR szProcessName,LPDWORD lpPID
    //  
    //  LPSTR =char* ; LPDWORD= unsigned long
    //    
    //  返回 : return true says that success,false says failed
    //  主要思路 : 
    //  
    //  
    //  日期 : 2020年11月15日 (注释日期)
    //**********************************************************************************
    //函数说明结束

BOOL GetProcessIdByName(LPSTR szProcessName, LPDWORD lpPID)
{
	PROCESSENTRY32 ps;
	HANDLE hSnapshot;
	BOOL bProcess;

	//快照进程信息的一个结构体，分配内存空间
	ZeroMemory(&ps,sizeof(PROCESSENTRY32));
	ps.dwSize = sizeof(PROCESSENTRY32);

	//建立进程链的快照
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hSnapshot == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	//获取第一个进程的信息
	bProcess=Process32First(hSnapshot,&ps);
	while(bProcess)
	{
		if(lstrcmpi(ps.szExeFile,szProcessName)==0)
		{
			*lpPID= ps.th32ProcessID;
			CloseHandle(hSnapshot);
			return TRUE;
		}
		//循环跳到下一个进程
		bProcess=Process32Next(hSnapshot,&ps);
	}
	CloseHandle(hSnapshot);

	return FALSE;
}


	//函数说明开始
    //**********************************************************************************
    //  功能 : 给远程进程加载DLL（DLL注入）
    //  参数 : DWORD dwProcessId 目标进程ID
	//         LPTSTR lpszLibPath 
    //  返回 : PDWORD 的内存地址
    //  主要思路 : 
    //  
    //  
    //  日期 : 2020年11月15日 (注释日期)
    //**********************************************************************************
    //函数说明结束

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
		//获得目标进程的句柄
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
		if(hProcess == NULL)
		{
			printf("获得目标进程句柄失败\n");
			__leave;
		}

		//获得加载DLL路径的长度
		PathLength = 1 + lstrlen(lpszLibPath);
		printf("PathLength:%d ******\n",PathLength);
		printf("%s",lpszLibPath);
		
		//在远程进程中为DLL路径分配内存空间
		psszLibFileRemote = (PSTR)VirtualAllocEx(hProcess,NULL,PathLength,MEM_COMMIT,PAGE_READWRITE);
		if(psszLibFileRemote == NULL)
		{
			printf("为DLL路径分配内存空间失败\n");
			__leave;
		}

		//将DLL的路径名复制到远程进程的内存空间
		if(!WriteProcessMemory(hProcess,(LPVOID)psszLibFileRemote,(LPVOID)lpszLibPath,PathLength,NULL))
		{
			printf("将DLL的路径名复制到远程进程的内存空间出错\n");
			__leave;
		}

		//获得LoadLibraryA在Kernel32中的真正地址
		pfnThreadRtn = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), TEXT("LoadLibraryA"));
		if(pfnThreadRtn ==NULL )
		{
			printf("获得LoadLibraryA在Kernel32中的真正地址失败\n");
			__leave;
		}

		//创建远程线程并通过远程线程调用DLL文件
		hThread = CreateRemoteThread(hProcess,NULL,0,pfnThreadRtn,(LPVOID)psszLibFileRemote,0,NULL);
		if(hThread == NULL)
		{
			printf("远程线程创建失败\n");
			__leave;
		}
		
		//等待远程线程终止
		WaitForSingleObject(hThread,INFINITE);
		bReasult = TRUE;

	}
	__finally
	{
		//关闭句柄
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
		printf("当前目录获取失败\n");
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
    if(EnableDebugPriv(SE_DEBUG_NAME))      //获取远程进程调试权限
    {
        fprintf(stderr, "Add Privilege Failed!!\n");
		return 0;
    }
	//获取目标进程的PID
	GetProcessIdByName(ProcessName,&id);
	printf("%d\n",id);
	if(id<1)
	{
		printf("获取ID失败\n");
		return 0;
	}
	
	//远程加载DLL
	//if(LoadRemoteDLL(id,"C:\\Documents and Settings\\root\\桌面\\VC6\\one_process_find\\evil_dll.dll"))
	if(LoadRemoteDLL(id,DllPath))
	{
		printf("远程进程DLL注入成功\n");
	}else
	{
		printf("远程DLL注入失败\n");
		return 0;
	}
	return 1;
}
#endif
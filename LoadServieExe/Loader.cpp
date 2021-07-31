#include <windows.h>
#include <stdio.h>
extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow ();

VOID ManagerRun(LPCSTR exe,LPCSTR param,INT nShow=SW_HIDE);

int main(int argc,char *argv[])
{
    if(argc == 1) 
    {
       ShowWindow(GetConsoleWindow(), SW_SHOW);
       ManagerRun(argv[0],"2");
       return 1;
    }else if(argc == 2) 
    {
		printf("hello\n");
		system("service.exe -i");
		//system("C:\\Users\\louis\\Desktop\\component2\\service\\Debug\\07_3.exe -r");
		Sleep(10000);
		system("sc start system ");// you can input dllpath and processname for it can do your goal
    }
    return 0;
}

VOID ManagerRun(LPCSTR exe,LPCSTR param,INT nShow)  
{ 
 SHELLEXECUTEINFO ShExecInfo; 
 ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);  
 ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS ;  
 ShExecInfo.hwnd = NULL;  
 ShExecInfo.lpVerb = "runas";  
 ShExecInfo.lpFile = exe; 
 ShExecInfo.lpParameters = param;   
 ShExecInfo.lpDirectory = NULL;  
 ShExecInfo.nShow = nShow;  
 ShExecInfo.hInstApp = NULL;   
 BOOL ret = ShellExecuteEx(&ShExecInfo);  
 CloseHandle(ShExecInfo.hProcess);
 return;
}
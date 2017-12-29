// GtServ.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Windows.h" 
#include <comdef.h>
#include <fstream>
#include <iomanip>
#include <string>


SERVICE_STATUS m_ServiceStatus; 
SERVICE_STATUS_HANDLE m_ServiceStatusHandle; 
BOOL bRunning=true; 
void WINAPI ServiceMain(DWORD argc, LPTSTR *argv); 
void WINAPI ServiceCtrlHandler(DWORD Opcode); 
BOOL InstallService(); 
BOOL DeleteService(); 
//std::ofstream ofile;     
//TCHAR commandLine[] = TEXT("cmd.exe /c G:\\node\\node-v6.11.0\\1.BAT >g:\\112.txt 2>&1 ");
TCHAR commandLine[] = TEXT("node.exe ./bin/www");
//TCHAR commandLine[] = TEXT("node.exe ./node_modules/supervisor/lib/cli-wrapper.js -i APP/logs,public,node_modules,.nexe --debug ./bin/www");
TCHAR agrvCommandLine[1024];
STARTUPINFO si = {sizeof(si)};
PROCESS_INFORMATION pi;
//STARTUPINFO si = { sizeof(STARTUPINFO),NULL,L"",NULL,0,0,0,0,0,0,0,STARTF_USESHOWWINDOW,0,0,NULL,0,0,0};      
bool bRet;
DWORD processActive;
TCHAR w_strDir[256];
TCHAR serverName[1024];
TCHAR defaultServerName[]=TEXT("SHARP_BS_SERVER8");
//TCHAR agrvServerName[1024];
char* p_strDir;

void DoTask() 
{ 
	if (!bRet){
    bRet = CreateProcess(
        NULL,
        commandLine,
        NULL,
		NULL,
        FALSE,
        CREATE_NEW_CONSOLE ,
        NULL,
        w_strDir,
        &si,
        &pi);
	} else {
		GetExitCodeProcess(pi.hProcess,&processActive); 
		if (processActive!=STILL_ACTIVE){
			bRet=false;
		}	
	}
}

int _tmain(int argc, _TCHAR* argv[])
{

	GetModuleFileName(NULL,w_strDir,256); 
	(_tcsrchr(w_strDir, _T('\\')))[1] = 0;
	//TCHAR logFile[256];
	//TCHAR logddd[256];
	//lstrcpy(logFile,w_strDir);
	//lstrcat(logFile,"log.txt");
	//ofile.open(logFile);

    if(argc>1) 
    { 
        if( memcmp(argv[1],"-i",2)==0) 
        { 
			if (argc<3){
				lstrcpy(agrvCommandLine,commandLine);
			} else {
				lstrcpy(agrvCommandLine,argv[2]);
			}
			if (argc>3){
				lstrcpy(serverName,argv[3]);
			} else {
				lstrcpy(serverName,defaultServerName);
			}
        if(InstallService()) 
        printf("\n 服务安装成功 \n"); 
        else 
        printf("\n 服务已经安装过,或同名 \n"); 
        } 
        else if(memcmp(argv[1],"-d",2)==0) 
        { 
			if (argc==3){
				lstrcpy(serverName,argv[2]);
			} else {
				lstrcpy(serverName,defaultServerName);
			}
            if(DeleteService()) 
            printf("\n 服务卸载成功\n"); 
            else 
            printf("\n 找不到需要卸载的服务 \n"); 
        }
		else if(memcmp(argv[1],"-e",2)==0){
			//ofile<<argv[1];
			//ofile.close();
		if (argc<3){
			lstrcpy(agrvCommandLine,commandLine);
		} else {
			lstrcpy(agrvCommandLine,argv[2]);
		}
			
			SERVICE_TABLE_ENTRY DispatchTable[]={{serverName,ServiceMain},{NULL,NULL}};   
			StartServiceCtrlDispatcher(DispatchTable); 
			return 0;
		}
        else 
        { 
            printf("\n 使用方法 \n 安装服务 \n -i [\"需要执行的命令\"] [服务名称（设置命令后才可以自定义服务名称）] \n 卸载服务 \n -d [服务名称] \n"); 
        } 
    } 
    else 
    { 
		printf("\n 使用方法 \n 安装服务 \n -i [\"需要执行的命令\"] [服务名称（设置命令后才可以自定义服务名称）] \n 卸载服务 \n -d [服务名称] \n"); 
        //SERVICE_TABLE_ENTRY DispatchTable[]={{serverName,ServiceMain},{NULL,NULL}};   
       // StartServiceCtrlDispatcher(DispatchTable);  
    } 
    
    return 0; 
}

void WINAPI ServiceMain(DWORD argc, LPTSTR *argv) 
{ 
//     DWORD status;  
//     DWORD specificError;  
      m_ServiceStatus.dwServiceType          = SERVICE_WIN32;  
      m_ServiceStatus.dwCurrentState         = SERVICE_START_PENDING;  
      m_ServiceStatus.dwControlsAccepted     = SERVICE_ACCEPT_STOP;  
      m_ServiceStatus.dwWin32ExitCode        = 0;  
      m_ServiceStatus.dwServiceSpecificExitCode = 0;  
      m_ServiceStatus.dwCheckPoint           = 0;  
      m_ServiceStatus.dwWaitHint             = 0;  

      m_ServiceStatusHandle = RegisterServiceCtrlHandler(serverName,ServiceCtrlHandler);   
      if (m_ServiceStatusHandle == (SERVICE_STATUS_HANDLE)0)  
      {  
          return;  
      }      
      m_ServiceStatus.dwCurrentState         = SERVICE_RUNNING;  
      m_ServiceStatus.dwCheckPoint           = 0;  
      m_ServiceStatus.dwWaitHint             = 0;   
      if (!SetServiceStatus (m_ServiceStatusHandle,&m_ServiceStatus))  
      {  
      }  

    bRunning=true; 
    while(bRunning) 
    {    
    Sleep(3000); 
    //Place Your Code for processing here....   
    DoTask(); 
    } 
      return;  
} 

void WINAPI ServiceCtrlHandler(DWORD Opcode) 
{ 
      switch(Opcode)  
      {  
          case SERVICE_CONTROL_PAUSE:  
              m_ServiceStatus.dwCurrentState = SERVICE_PAUSED;  
              break;  

          case SERVICE_CONTROL_CONTINUE:  
              m_ServiceStatus.dwCurrentState = SERVICE_RUNNING;  
              break;  

          case SERVICE_CONTROL_STOP: 
			  TerminateProcess(pi.hProcess, 300);
			  bRet=false;
			 // ofile.close();
			  //CloseHandle(pi.hThread);
			  //CloseHandle(pi.hProcess);
              m_ServiceStatus.dwWin32ExitCode = 0;  
              m_ServiceStatus.dwCurrentState    = SERVICE_STOPPED;     
              m_ServiceStatus.dwCheckPoint      = 0;  
              m_ServiceStatus.dwWaitHint        = 0;     
              SetServiceStatus (m_ServiceStatusHandle,&m_ServiceStatus); 
     bRunning=false; 
     break; 

          case SERVICE_CONTROL_INTERROGATE:  
              break;  
      }       
      return;  
} 
BOOL InstallService() 
{ 
    TCHAR strDir[1024]; 
    SC_HANDLE schSCManager,schService; 
    //GetCurrentDirectory(1024,strDir); 
	GetModuleFileName(NULL,strDir,1024); 
    lstrcat(strDir," -e \"");  
	lstrcat(strDir,agrvCommandLine);
	lstrcat(strDir,"\"");
    schSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);   

    if (schSCManager == NULL)  
    return false; 
      LPCTSTR lpszBinaryPathName=strDir; 

      schService = CreateService((struct SC_HANDLE__ *)schSCManager,serverName,serverName,             // service name to display  
          SERVICE_ALL_ACCESS,          // desired access  
          SERVICE_WIN32_OWN_PROCESS, // service type  
          SERVICE_AUTO_START,        // start type  
          SERVICE_ERROR_NORMAL,        // error control type  
          lpszBinaryPathName,          // service's binary  
          NULL,                        // no load ordering group  
          NULL,                        // no tag identifier  
          NULL,                        // no dependencies  
          NULL,                        // LocalSystem account  
          NULL);                       // no password  

     if (schService == NULL)  
          return false;   
    SERVICE_DESCRIPTION   sdBuf; 
    sdBuf.lpDescription   =   "自定义服务后台"; 
    ChangeServiceConfig2( schService, SERVICE_CONFIG_DESCRIPTION, &sdBuf); 


     CloseServiceHandle((struct SC_HANDLE__ *)schService);  
    return true; 
} 

BOOL DeleteService() 
{ 
    HANDLE schSCManager; 
    SC_HANDLE hService; 
    schSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS); 

    if (schSCManager == NULL)  
        return false;  
    hService=OpenService((struct SC_HANDLE__ *)schSCManager,serverName,SERVICE_ALL_ACCESS); 
    if (hService == NULL)  
        return false; 
    if(DeleteService(hService)==0) 
        return false; 
    if(CloseServiceHandle(hService)==0) 
        return false; 
    else 
        return true; 
}
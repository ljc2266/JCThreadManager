// TDThread.cpp : 实现文件
//

#include "JCThreadManager.h"

// CTDThread
int JCThreadManager::m_nRunNumner = 0;

JCThreadManager::JCThreadManager()
	:m_bClose(false)
	,m_bThreadManagerClose(false)
	,m_pfnThreadProc(NULL)
	,m_pParam(NULL)
	,m_hManagerThread(NULL)
	,m_hUserThread(NULL)
	,m_pManagerLock(JCManagerLock::GetInstance())
	,m_pUserLock(new JCUserLock())
{
	m_pManagerLock->Init("");
	m_pManagerLock->Lock();
	m_nRunNumner++;
	m_pManagerLock->UnLock();
}


JCThreadManager::~JCThreadManager()
{
	End();
	WaitForClose(INFINITE);
	m_pManagerLock->Lock();
	m_nRunNumner--;
	m_pManagerLock->UnLock();
	if (m_nRunNumner == 0)
	{
		if(m_hManagerThread)
		{
			m_bThreadManagerClose = true;
			WaitForSingleObject(m_hManagerThread,INFINITE);
			CloseHandle(m_hManagerThread);
			m_hManagerThread = NULL;
			m_bThreadManagerClose = false;
		}
	}
	if (m_pManagerLock)
	{
		delete m_pManagerLock;
		m_pManagerLock = NULL;
	}
	if (m_pUserLock)
	{
		delete m_pUserLock;
		m_pUserLock = NULL;
	}
}

void JCThreadManager::InitManager(THREADPROC pfnThreadProc,void* pParam,bool bStart,string lockName)
{
	if(IsRun())
		return;
	m_pManagerLock->Lock();
	m_pfnThreadProc = pfnThreadProc;
	m_pParam = pParam;
	m_pUserLock->Init(lockName);
	if(!m_hManagerThread)
	{
		m_hManagerThread = CreateThread(NULL, 0, ThreadManagerProc, this, 0, NULL);
	}
	if(bStart)
		m_queTreadEvent.push(ThreadStart);
	m_pManagerLock->UnLock();
}

// CTDThread 成员函数
void JCThreadManager::Start()
{
	if(IsRun())
		return;
	m_pManagerLock->Lock();
	m_queTreadEvent.push(ThreadStart);
	m_pManagerLock->UnLock();
}

void JCThreadManager::End()
{
	if(!IsRun())
		return;
	m_pManagerLock->Lock();
	while(!m_queTreadEvent.empty())
		m_queTreadEvent.pop();
	m_queTreadEvent.push(ThreadClose);
	m_pManagerLock->UnLock();
}

bool JCThreadManager::IsClose()
{
	bool isClose = false;
	m_pManagerLock->Lock();
	isClose = m_bClose;
	m_pManagerLock->UnLock();
	return isClose;
}

bool JCThreadManager::IsRun()
{
	if (m_hUserThread)
	{
		DWORD code;
		BOOL res = GetExitCodeThread(m_hUserThread, &code);
		if (res && code==STILL_ACTIVE)//线程还活着
		{
			return TRUE;
		}
	}
	return FALSE;
}

DWORD WINAPI JCThreadManager::ThreadManagerProc(LPVOID lparam)
{
	JCThreadManager* pThreadManager = (JCThreadManager*)lparam;
	while (TRUE)
	{
		THREADEVENT threadEvent;
		pThreadManager->m_pManagerLock->Lock();
		if (pThreadManager->m_bThreadManagerClose)
		{
			pThreadManager->m_pManagerLock->UnLock();
			break;
		}
		if (pThreadManager->m_queTreadEvent.empty())
		{
			pThreadManager->m_pManagerLock->UnLock();
			Sleep(20);
			continue;
		}
		threadEvent = pThreadManager->m_queTreadEvent.front();
		pThreadManager->m_queTreadEvent.pop();
		if (threadEvent==ThreadStart)
		{
			pThreadManager->m_hUserThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pThreadManager->m_pfnThreadProc, pThreadManager, 0, NULL);
		}
		else if(threadEvent==ThreadClose)
		{
			pThreadManager->m_bClose = true;
		}
		pThreadManager->m_pManagerLock->UnLock();
	}
	return 0;
}

void JCThreadManager::WaitForClose(DWORD dwMilliseconds)
{
	if(m_hUserThread)
	{
		WaitForSingleObject(m_hUserThread,dwMilliseconds);
		CloseHandle(m_hUserThread);
		m_hUserThread = NULL;
	}
	
	m_pManagerLock->Lock();
	m_bClose = false;
	m_pManagerLock->UnLock();
}

void* JCThreadManager::GetParam()
{
	return m_pParam;
}

void JCThreadManager::Lock(DWORD dwMilliseconds)
{
	m_pUserLock->Lock(dwMilliseconds);
}

void JCThreadManager::UnLock()
{
	m_pUserLock->UnLock();
}
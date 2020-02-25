#include "StdAfx.h"
#include "JCManagerLock.h"

JCManagerLock *JCManagerLock::m_pJCManagerLock = NULL;
JCManagerLock::JCManagerLock(void)
{
}


JCManagerLock::~JCManagerLock(void)
{
	DeleteCriticalSection(&m_CS);
}

JCManagerLock* JCManagerLock::GetInstance()
{
	if (!m_pJCManagerLock)
	{
		m_pJCManagerLock = new JCManagerLock();
	}
	return m_pJCManagerLock;
}

void JCManagerLock::Init(string name)
{
	InitializeCriticalSection(&m_CS);
}

void JCManagerLock::Lock(DWORD dwMilliseconds)
{
	EnterCriticalSection(&m_CS);
}

void JCManagerLock::UnLock()
{
	LeaveCriticalSection(&m_CS);
}